#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <random>
#include <windows.h>  
#include <locale.h>
#include "protocol.h"

#define UNICODE  
#include <sqlext.h>  
#include <string>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;


enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_NPC_RUSH, OP_CREATE_CUBE, OP_UPDATE };

enum SESSION_STATE { ST_FREE, ST_ACCEPTED, ST_INGAME, ST_SLEEP };

class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	int target_id;

	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}

	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class SESSION {
	OVER_EXP _recv_over;
public:
	mutex	_sl;
	SESSION_STATE _s_state;
	int _id;
	SOCKET _socket;
	char	_name[NAME_SIZE];
	int		_prev_remain;

public:
	SESSION()
	{
		_id = -1;
		_socket = 0;
		_name[0] = 0;
		_s_state = ST_FREE;
		_prev_remain = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
	void send_login_ok_packet(int c_id);
};

void SESSION::send_login_ok_packet(int c_id)
{
	SC_LOGIN_OK_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	do_send(&p);
}

array<SESSION, MAX_USER> clients;

HANDLE g_h_iocp;
SOCKET g_s_socket;
SOCKET ss_socket;

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	int receiveBytes;
	int sendBytes;
};

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
		clients[c_id].send_login_ok_packet(c_id % 4);
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
			/*if (pl._Room_Num == clients[c_id]._Room_Num) {
				pl.send_add_object(c_id % 4, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id]._name);
				clients[c_id].send_add_object(pl._id % 4, pl.x, pl.y, pl.z, pl.degree, pl._name);
				pl.room_list.insert(c_id);
				clients[c_id].room_list.insert(pl._id);
			}*/
			pl._sl.unlock();
		}

		/*if (clients[c_id].room_list.size() == 0) {
			for (int i = clients[c_id]._Room_Num * 10 + MAX_USER; i < clients[c_id]._Room_Num * 10 + MAX_USER + 9; i++) {
				add_timer(i, 5000, EV_MOVE, i);
			}
			add_timer((clients[c_id]._Room_Num * 10) + MAX_USER + 9, 10000, EV_CK, 0);
		}

		for (int i = MAX_USER; i < MAX_USER + NPC_NUM; i++) {
			if (clients[c_id]._Room_Num == clients[i]._Room_Num) {
				clients[c_id].room_list.insert(i);
				clients[i].room_list.insert(c_id);
				clients[c_id].send_add_object((i - MAX_USER) % 10 + 4, clients[i].x, clients[i].y, clients[i].z, clients[i].degree, clients[i]._name);
			}
		}*/

		break;
	}
	case CS_MOVE: {
		cout << "move 지원 안함" << endl;
		/*CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;
		clients[c_id].degree = p->degree;*/
		break;
	}
	case CS_CONSOLE: {
		cout << "콘솔 지원 안함" << endl;
		//reset_lua(c_id);
		break;
	}
	case SS_CONNECT_SERVER: {
		cout << "쌍방 연결 확인" << endl;

		SS_DATA_PASS_PACKET p;
		p.size = sizeof(SS_DATA_PASS_PACKET);
		p.type = SS_DATA_PASS;
		p.num = 12345;

		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(&p) };
		WSAOVERLAPPED over;
		char send_buf[BUF_SIZE];

		sdata->_wsabuf.len = p.size;
		sdata->_wsabuf.buf = send_buf;
		ZeroMemory(&over, sizeof(over));
		sdata->_comp_type = OP_SEND;
		memcpy(send_buf, &p, p.size);

		WSASend(ss_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
		break;
	}
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
		}
	}
}

int main()
{
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

	g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), g_h_iocp, 9999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	a_over._wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
	AcceptEx(g_s_socket, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

	// 게임 서버와 커넥트
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	ss_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(GAME_SERVER_PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(GAME_SERVER_IP);

	if (connect(ss_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cout << "커넥트 실패" << endl;
	}
	else {
		cout << "커넥트 성공" << endl;
		
		SS_CONNECT_SERVER_PACKET p;
		p.size = sizeof(SS_CONNECT_SERVER_PACKET);
		p.type = SS_CONNECT_SERVER;
		p.id = 0;

		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(&p) };
		WSAOVERLAPPED over;
		char send_buf[BUF_SIZE];

		sdata->_wsabuf.len = p.size;
		sdata->_wsabuf.buf = send_buf;
		ZeroMemory(&over, sizeof(over));
		sdata->_comp_type = OP_SEND;
		memcpy(send_buf, &p, p.size);

		WSASend(ss_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}


	vector <thread> worker_threads;
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(do_worker);

	for (auto& th : worker_threads)
		th.join();

	closesocket(g_s_socket);
	WSACleanup();
}