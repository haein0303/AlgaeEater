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
	int		_state;
	int		_prev_remain;

public:
	SESSION()
	{
		_id = -1;
		_socket = 0;
		_name[0] = 0;
		_s_state = ST_FREE;
		_state = 0;
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
queue<int> match_list;

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
	case LCS_LOGIN: {
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

		LSC_LOGIN_OK_PACKET p;
		p.id = c_id;
		p.size = sizeof(LSC_LOGIN_OK_PACKET);
		p.type = LSC_LOGIN_OK;

		clients[c_id].do_send(&p);
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._sl.unlock();
		break;
	}
	case LCS_MATCH: {
		clients[c_id]._state = 1;
		match_list.push(c_id);
		break;
	}
	case SS_CONNECT_SERVER: {
		cout << "쌍방 연결 확인" << endl;

		SS_CONNECT_SERVER_PACKET* p = reinterpret_cast<SS_CONNECT_SERVER_PACKET*>(packet);
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
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._sl.unlock();

		SS_DATA_PASS_PACKET pi;
		pi.size = sizeof(SS_DATA_PASS_PACKET);
		pi.type = SS_DATA_PASS;
		strcpy_s(pi.name, "lobby");

		clients[c_id].do_send(&pi);

		break;
	}
	}
}

enum EVENT_TYPE { EV_MATCH };
mutex timer_l;


using namespace chrono;

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

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id)
{
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
			case EV_MATCH:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_UPDATE;
				ex_over->target_id = ev.object_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				add_timer(ev.object_id, 100, ev.ev, ev.target_id);
				break;
			}
			default:
				break;
			}
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
				clients[client_id]._state = 0;
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
		case OP_UPDATE:
			if (match_list.size() >= 4) {
				// 만약에 겜서버 준비가 필요하면 여기서 하라고 보내줘야 함
				for (int i = 0; i < 4; ++i) {
					LSC_CONGAME_PACKET p;
					p.connect = true;
					p.size = sizeof(LSC_CONGAME_PACKET);
					p.type = LSC_CONGAME;
					clients[match_list.front()].do_send(&p);
					match_list.pop();
				}
			}
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
	server_addr.sin_port = htons(LOBBY_SERVER_PORT_NUM);
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


	match_list.empty();
	add_timer(0, 1000, EV_MATCH, 0);

	// 게임 서버와 커넥트

	ss_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(GAME_SERVER_PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(GAME_SERVER_IP);

	int client_id;

	if (connect(ss_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cout << "커넥트 실패" << endl;
	}
	else {
		cout << "커넥트 성공" << endl;
		OVER_EXP ss_over;
		ss_over._comp_type = OP_ACCEPT;
		ss_over._wsabuf.buf = reinterpret_cast<CHAR*>(ss_socket);

		SOCKET c_socket = reinterpret_cast<SOCKET>(ss_over._wsabuf.buf);
		client_id = get_new_client_id();
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

		ZeroMemory(&ss_over._over, sizeof(ss_over._over));
		ss_over._wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
		int addr_size = sizeof(SOCKADDR_IN);
		AcceptEx(g_s_socket, c_socket, ss_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &ss_over._over);


		SS_CONNECT_SERVER_PACKET p;
		p.size = sizeof(SS_CONNECT_SERVER_PACKET);
		p.type = SS_CONNECT_SERVER;
		strcpy_s(p.name, "lobby");

		clients[client_id].do_send(&p);
	}


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