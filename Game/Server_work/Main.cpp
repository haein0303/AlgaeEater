#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <unordered_set>
#include <windows.h>
#include <string>
#include <queue>
#include <locale.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <random>

#include "protocol.h"
#include "Over_EXP.h"
#include "Session.h"

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment (lib, "lua54.lib")

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

HANDLE g_h_iocp;
SOCKET g_s_socket;

default_random_engine dre;
uniform_int_distribution<> uid{ 0, 3 };

array<SESSION, MAX_USER + NPC_NUM> clients;

struct CUBE {
public:
	float x, y, z;
};

array<CUBE, 2> cubes;

void disconnect(int c_id);

enum EVENT_TYPE { EV_MOVE, EV_RUSH, EV_CK };

struct TIMER_EVENT {
	int object_id;
	EVENT_TYPE ev;
	chrono::system_clock::time_point act_time;
	int target_id;

	constexpr bool operator < (const TIMER_EVENT& _Left) const
	{
		return (act_time > _Left.act_time);
	}

};

priority_queue<TIMER_EVENT> timer_queue;
mutex timer_l;

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id)
{
	using namespace chrono;
	TIMER_EVENT ev;
	ev.act_time = system_clock::now() + milliseconds(act_time);
	ev.object_id = obj_id;
	ev.ev = e_type;
	ev.target_id = target_id;

	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
}

void do_timer()
{
	while (true)
	{
		this_thread::sleep_for(1ms);
		while (true) 
		{
			timer_l.lock();

			if (timer_queue.empty() == true) 
			{
				timer_l.unlock();
				break;
			}

			if (timer_queue.top().act_time > chrono::system_clock::now()) 
			{
				timer_l.unlock();
				break;
			}

			TIMER_EVENT ev = timer_queue.top();
			timer_queue.pop();
			timer_l.unlock();

			switch (ev.ev)
			{
				case EV_MOVE: 
				{
					auto ex_over = new OVER_EXP;
					ex_over->_comp_type = OP_NPC_MOVE;
					ex_over->target_id = ev.object_id;
					PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
					add_timer(ev.object_id, 100, ev.ev, ev.target_id);
					break;
				}
				case EV_RUSH:
				{
					auto ex_over = new OVER_EXP;
					ex_over->_comp_type = OP_NPC_RUSH;
					ex_over->target_id = ev.target_id;
					PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
					break;
				}
				case EV_CK:
				{
					srand((unsigned int)time(NULL));
					int rd_id = rand() % MAX_USER;

					if (clients[rd_id]._s_state == ST_INGAME) {
						lua_getglobal(clients[19].L, "event_rush");
						lua_pushnumber(clients[19].L, rd_id);
						lua_pcall(clients[19].L, 1, 0, 0);
					}
					else {
						add_timer(19, 10, EV_CK, 0);
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i]._sl.lock();
		if (clients[i]._s_state == ST_FREE) {
			clients[i]._s_state = ST_ACCEPTED;
			clients[i]._sl.unlock();
			return i;
		}
		clients[i]._sl.unlock();
	}
	return -1;
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		clients[c_id]._sl.lock();
		if (clients[c_id]._s_state == ST_FREE) {
			clients[c_id]._sl.unlock();
			break;
		}
		if (clients[c_id]._s_state == ST_INGAME) {
			clients[c_id]._sl.unlock();
			disconnect(c_id);
			break;
		}

		strcpy_s(clients[c_id]._name, p->name);
		clients[c_id].x = 0;
		clients[c_id].y = 0;
		clients[c_id].z = 0;
		clients[c_id].degree = 0;
		clients[c_id].send_login_ok_packet(c_id, 0, 0, 0, 0);
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._sl.unlock();

		for (int i = 0; i < MAX_USER; ++i) {
			auto& pl = clients[i];
			if (pl._id == c_id) continue;
			pl._sl.lock();
			if (ST_INGAME != pl._s_state) {
				pl._sl.unlock();
				continue;
			}
			pl.send_add_object(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id]._name);
			clients[c_id].send_add_object(pl._id, pl.x, pl.y, pl.z, pl.degree, pl._name);
			pl._sl.unlock();
		}

		for (int i = MAX_USER; i < MAX_USER + NPC_NUM; i++) {
			clients[c_id].send_add_object(i, clients[i].x, clients[i].y, clients[i].z, clients[i].degree, clients[i]._name);
		}

		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		float x = p->x;
		float y = p->y;
		float z = p->z;
		float degree = p->degree;
		/*switch (p->direction) {
		case 0:
		{
			y--;
			break;
		}
		case 1:
		{
			y++;
			break;
		}
		case 2:
		{
			x--;
			break;
		}
		case 3:
		{
			x++;
			break;
		}
		}*/

		clients[c_id].x = x;
		clients[c_id].y = y;
		clients[c_id].z = z;
		clients[c_id].degree = degree;
		
		//clients[c_id].send_move_packet(c_id, x, y);

		for (auto& pl : clients) {
			if (pl._id == c_id) continue;
			pl._sl.lock();
			if (ST_INGAME != pl._s_state) {
				pl._sl.unlock();
				continue;
			}
			pl.send_move_packet(c_id, x, y, z, degree);
			pl._sl.unlock();
		}
		break;
	}
	}
}

void rush_npc(int player_id) 
{
	float x = clients[19].x;
	float z = clients[19].z;

	Sleep(10);

	if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 3) {
		add_timer(19, 5000, EV_RUSH, 0);
		return;
	}

	for (int i = 0; i < 2; i++) {
		if (abs(x - cubes[i].x) + abs(z - cubes[i].z) <= 3) {
			cout << "±âµÕ Ãæµ¹" << endl;
			add_timer(19, 10000, EV_RUSH, 0);
			return;
		}
	}

	if (x > clients[player_id].x) x--;
	else if(x < clients[player_id].x) x++;

	if (z > clients[player_id].z) z--;
	else if (z < clients[player_id].z) z++;

	if (abs(x - clients[player_id].x) < 1) x = clients[player_id].x;
	if (abs(z - clients[player_id].z) < 1) z = clients[player_id].z;

	clients[19].x = x;
	clients[19].z = z;

	for (int i = 0; i < MAX_USER; ++i) {
		auto& pl = clients;
		pl[i].send_move_packet(19, clients[19].x, clients[19].y, clients[19].z, clients[19].degree);
	}

	rush_npc(player_id);
}

void move_npc(int npc_id)
{
	float z = clients[npc_id].z;
	float x = clients[npc_id].x;

	if (clients[npc_id].move_stack == 10) {
		clients[npc_id].move_stack = 0;
		clients[npc_id].move_degree = 0;
	}

	if (clients[npc_id].move_degree == 0) {
		clients[npc_id].move_degree = uid(dre);
	}

	if (clients[npc_id].move_stack != 10) {
		switch (clients[npc_id].move_degree)
		{
		case 0:
			x++;
			break;
		case 1:
			x--;
			break;
		case 2:
			z++;
			break;
		case 3:
			z--;
			break;
		default:
			break;
		}
		clients[npc_id].move_stack++;
	}

	clients[npc_id].z = z;
	clients[npc_id].x = x;

	for (int i = 0; i < MAX_USER;++i) {
		auto& pl = clients;
		pl[i].send_move_packet(npc_id, clients[npc_id].x, clients[npc_id].y, clients[npc_id].z, clients[npc_id].degree);
	}
}

void disconnect(int c_id)
{
	clients[c_id]._sl.lock();
	if (clients[c_id]._s_state == ST_FREE) {
		clients[c_id]._sl.unlock();
		return;
	}
	closesocket(clients[c_id]._socket);
	clients[c_id]._s_state = ST_FREE;
	clients[c_id]._sl.unlock();

	for (auto& pl : clients) {
		if (pl._id == c_id) continue;
		pl._sl.lock();
		if (pl._s_state != ST_INGAME) {
			pl._sl.unlock();
			continue;
		}
		SC_REMOVE_OBJECT_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_OBJECT;
		pl.do_send(&p);
		pl._sl.unlock();
	}
}

void do_worker()
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(g_h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		int client_id = static_cast<int>(key);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			SOCKET c_socket = reinterpret_cast<SOCKET>(ex_over->_wsabuf.buf);
			int client_id = get_new_client_id();
			if (client_id != -1) {
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, client_id, 0);
				clients[client_id].do_recv();
				c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else cout << "Max user exceeded.\n";

			ZeroMemory(&ex_over->_over, sizeof(ex_over->_over));
			ex_over->_wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, c_socket, ex_over->_send_buf, 0, addr_size + 16, addr_size + 16, 0, &ex_over->_over);
			break;
		}
		case OP_RECV: {
			if (0 == num_bytes) disconnect(client_id);
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			if (0 == num_bytes) disconnect(client_id);
			delete ex_over;
			break;

		case OP_NPC_MOVE:
		{
			move_npc(ex_over->target_id);
			delete ex_over;
			break;
		}
		case OP_NPC_RUSH:
		{
			rush_npc(ex_over->target_id);
			delete ex_over;
			break;
		}
		}
	}
}

int API_get_x(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float x = clients[obj_id].x;

	lua_pushnumber(L, x);
	return 1;
}

int API_get_z(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float z = clients[obj_id].z;

	lua_pushnumber(L, z);
	return 1;
}

int API_Rush(lua_State* L)
{
	int client_id = lua_tonumber(L, -2);
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 3);

	add_timer(npc_id, 5000, EV_RUSH, client_id);
	return 0;
}

int API_get_state(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	int state = clients[obj_id]._s_state;

	lua_pushnumber(L, state);
	return 1;
}

void initialize_npc()
{
	for (int i = MAX_USER; i < MAX_USER + NPC_NUM -1; ++i) {
		clients[i]._s_state = ST_INGAME;
		clients[i].x = (i - 10) * 1.5;
		clients[i].y = 0;
		clients[i].z = 0;
		clients[i].degree = 0;
		clients[i].move_stack = 0;
		clients[i].move_degree = 0;
		clients[i]._name[0] = 0;
		clients[i]._prev_remain = 0;
		add_timer(i, 100, EV_MOVE, i);
	}

	clients[19]._s_state = ST_INGAME;
	clients[19].x = (19 - 10) * 1.5;
	clients[19].y = 0;
	clients[19].z = 0;
	clients[19].degree = 0;
	clients[19].move_stack = 0;
	clients[19].move_degree = 0;
	clients[19]._name[0] = 0;
	clients[19]._prev_remain = 0;

	lua_State* L = luaL_newstate();
	clients[19].L = L;

	luaL_openlibs(L);
	luaL_loadfile(L, "hello.lua");
	lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_object_id");
	lua_pushnumber(L, 19);
	lua_pcall(L, 1, 0, 0);

	lua_register(L, "API_get_x", API_get_x);
	lua_register(L, "API_get_z", API_get_z);
	lua_register(L, "API_Rush", API_Rush);
	lua_register(L, "API_get_state", API_get_state);

	add_timer(19, 10000, EV_CK, 0);
}

void initialize_cube() 
{
	for (int i = 0; i < 2; i++) {
		cubes[i].x = -2.0f + i * 4.0f;
		cubes[i].y = 2.0f;
		cubes[i].z = -5.0f;
	}
}

int main()
{
	initialize_npc();
	initialize_cube();
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
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(do_worker);

	thread timer_thread{ do_timer };
	timer_thread.join();

	for (auto& th : worker_threads)
		th.join();

	closesocket(g_s_socket);
	WSACleanup();
}