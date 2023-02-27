
#include "stdafx.h"
#include "util.h"
#include "Session.h"
#include "Timer.h"
#include "Lua_API.h"
#include "NPC.h"

extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;	
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

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
		clients[c_id].send_login_ok_packet(c_id % 4, 0, 0, 0, 0);
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._Room_Num = c_id / 4;
		clients[c_id].room_list.clear();
		clients[c_id]._sl.unlock();

		for (int i = 0; i < MAX_USER; ++i) {
			auto& pl = clients[i];
			if (pl._id == c_id) continue;
			pl._sl.lock();
			if (ST_INGAME != pl._s_state) {
				pl._sl.unlock();
				continue;
			}
			if (pl._Room_Num == clients[c_id]._Room_Num) {
				pl.send_add_object(c_id % 4, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id]._name);
				clients[c_id].send_add_object(pl._id % 4, pl.x, pl.y, pl.z, pl.degree, pl._name);
				pl.room_list.insert(c_id);
				clients[c_id].room_list.insert(pl._id);
			}
			pl._sl.unlock();
		}

		if (clients[c_id].room_list.size() == 0) {
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
		}

		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;
		clients[c_id].degree = p->degree;
		break;
	}
	case CS_CONSOLE: {
		reset_lua(c_id);
		break;
	}
	case SS_CONNECT_SERVER: {
		cout << "SS_CONNECT_SERVER 로비 서버 커넥트" << endl;

		// 게임 서버와 커넥트
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);

		SOCKET ss_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		SOCKADDR_IN serverAddr;
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(LOBBY_SERVER_PORT_NUM);
		serverAddr.sin_addr.S_un.S_addr = inet_addr(LOBBY_SERVER_IP);

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
		break;
	}
	case SS_DATA_PASS: {
		cout << "데이터 들어옴" << endl;
		SS_DATA_PASS_PACKET* p = reinterpret_cast<SS_DATA_PASS_PACKET*>(packet);

		cout << p->num << endl;
		break;
	}
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

	if (clients[c_id].room_list.size() != 0) {

		for (auto& pl : clients[c_id].room_list) {
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			SC_REMOVE_OBJECT_PACKET p;
			p.id = c_id;
			p.size = sizeof(p);
			p.type = SC_REMOVE_OBJECT;
			clients[pl].do_send(&p);
			clients[pl]._sl.unlock();
			clients[pl].room_list.erase(c_id);
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
				//disconnect(static_cast<int>(key));
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
				clients[client_id].z = 0;
				clients[client_id].degree = 0;
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
		case OP_SEND: {
			if (0 == num_bytes) disconnect(client_id);
			delete ex_over;
			break;
		}
		case OP_NPC_MOVE: {
			move_npc(ex_over->target_id);
			delete ex_over;
			break;
		}
		case OP_NPC_RUSH: {
			rush_npc(ex_over->target_id, key);
			delete ex_over;
			break;
		}
		case OP_CREATE_CUBE: {
			send_cube(ex_over->target_id, clients[ex_over->target_id].x, clients[ex_over->target_id].y, clients[ex_over->target_id].z);
			delete ex_over;
			break;
		}
		case OP_UPDATE: {
			Update_Player();
			add_timer(0, 33, EV_UP, 0);
			delete ex_over;
			break;
		}
		}
	}
}

void Update_Player()
{
	for (int i = 0; i < MAX_USER; i++) {
		for (auto& pl : clients[i].room_list) {
			clients[i]._sl.lock();
			if (clients[i]._s_state != ST_INGAME) {
				clients[i]._sl.unlock();
				continue;
			}
			clients[i]._sl.unlock();

			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			if (pl < 400)
				clients[i].send_move_packet(pl % 4, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree);
			else
				clients[i].send_move_packet((pl - MAX_USER) % 10 + 4, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree);
		}
	}
}