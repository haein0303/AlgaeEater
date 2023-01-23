#pragma once
#include "Extern.h"

extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;
extern lua_State* L;
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, 4> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

void disconnect(int c_id);

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
	case CS_CONSOLE: {
		reset_lua();
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
		case OP_CREATE_CUBE:
		{
			initialize_cube(clients[19].x, clients[19].y, clients[19].z);
			delete ex_over;
			break;
		}
		}
	}
}