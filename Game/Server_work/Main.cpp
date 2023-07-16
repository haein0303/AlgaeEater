#include "stdafx.h"

#include "util.h"

#include "protocol.h"
#include "Over_EXP.h"
#include "Session.h"
#include "Timer.h"
#include "Lua_API.h"
#include "NPC.h"

array<SESSION, MAX_USER + NPC_NUM> clients;

array<CUBE, CUBE_NUM> cubes;

array<KEY, KEY_NUM> keys;

array<FIELD, FIELD_NUM> fields;

HANDLE g_h_iocp;
SOCKET g_s_socket;

default_random_engine dre;
uniform_int_distribution<> uid{ 0, 3 };

priority_queue<TIMER_EVENT> timer_queue;
mutex timer_l;

int main()
{
	stage();
	initialize_key();
	initialize_cube();
	initialize_npc();
	initialize_field();

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), g_h_iocp, 9999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	a_over._wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
	AcceptEx(g_s_socket, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

	vector <thread> worker_threads;
	for (int i = 0; i < 10; ++i)
		worker_threads.emplace_back(do_worker);

	thread timer_thread{ do_timer };
	timer_thread.join();

	for (auto& th : worker_threads)
		th.join();

	closesocket(g_s_socket);
	WSACleanup();
}