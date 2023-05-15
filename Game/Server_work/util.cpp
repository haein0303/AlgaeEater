
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
		clients[c_id].char_state = 0;
		clients[c_id].color = 0;
		clients[c_id].hp = 100;
		clients[c_id].send_login_ok_packet(c_id, 0, 0, 0, 0, 100);
		clients[c_id]._s_state = ST_INGAME;
		if (clients[0]._Room_Num != 999) clients[c_id]._Room_Num = c_id / 4;
		else clients[c_id]._Room_Num = (c_id - 1) / 4;
		clients[c_id].room_list.clear();
		//clients[c_id].stage = p->stage;
		clients[c_id].stage = 0;
		clients[c_id]._sl.unlock();

		// 다른 플레이어 업데이트 받는 부분
		for (int i = 0; i < MAX_USER; ++i) {
			auto& pl = clients[i];
			if (pl._id == c_id) continue;
			pl._sl.lock();
			if (ST_INGAME != pl._s_state) {
				pl._sl.unlock();
				continue;
			}
			if (pl._Room_Num == clients[c_id]._Room_Num) {
				pl.send_add_object(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, 
					clients[c_id]._name, clients[c_id].hp, clients[c_id].char_state);
				clients[c_id].send_add_object(pl._id, pl.x, pl.y, pl.z, pl.degree, pl._name, pl.hp, pl.char_state);
				pl.room_list.insert(c_id);
				clients[c_id].room_list.insert(pl._id);
			}
			pl._sl.unlock();
		}

		// npc 세팅 부분
		if (clients[c_id]._Room_Num != 999) {
			if (clients[c_id].room_list.size() == 0 && clients[clients[c_id]._Room_Num * 10 + MAX_USER + 9].Lua_on == false) {
				for (int i = clients[c_id]._Room_Num * 10 + MAX_USER; i < clients[c_id]._Room_Num * 10 + MAX_USER + 10; i++) {
					switch (clients[c_id].stage)
					{
					case 0: // 테스트 스테이지
						if ((i - MAX_USER) % 10 < 5) {
							clients[i].x = 40;
							clients[i].start_x = 40;
							clients[i].z = -50 + (i - MAX_USER) * 20;
							clients[i].start_z = -50 + (i - MAX_USER) * 20;
						}
						else {
							clients[i].x = -150 + (i - MAX_USER) * 20;
							clients[i].start_x = -150 + (i - MAX_USER) * 20;
							clients[i].z = 40;
							clients[i].start_z = 40;
						}
						break;
					case 1:	// 스테이지 1
						break;
					default:
						break;
					}

					add_timer(i, 10000, EV_NPC_CON, c_id);
					clients[i].Lua_on = true;

				}
				add_timer(clients[c_id]._Room_Num * 10 + MAX_USER + 9, 10000, EV_CK, c_id);
				//clients[clients[c_id]._Room_Num * 10 + MAX_USER + 9].Lua_on = true;
			}
		}

		for (int i = MAX_USER; i < MAX_USER + NPC_NUM; i++) {
			if (clients[c_id]._Room_Num == clients[i]._Room_Num) {
				clients[c_id].room_list.insert(i);
				clients[i].room_list.insert(c_id);
				clients[c_id].send_add_object(i, clients[i].x, clients[i].y, clients[i].z, clients[i].degree, clients[i]._name, clients[i].hp, clients[i].char_state);
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
		clients[c_id].char_state = p->char_state;
		clients[c_id].client_time = p->client_time;
		//if (c_id == 0) {
		//	cout << "RECV : " << p->client_time << endl;
		//}
		Update_Player(c_id);
		break;
	}
	case CS_CONSOLE: {
		reset_lua(c_id);
		break;
	}
	case CS_COLLISION: {
		CS_COLLISION_PACKET* p = reinterpret_cast<CS_COLLISION_PACKET*>(packet);
		int npc_id = 0;
		if (p->attacker_id < MAX_USER) {	// 공격자가 플레이어
			clients[p->target_id].hp -= 10;
			if (clients[p->target_id].hp == 0) {
				clients[p->target_id].char_state = 4;
				clients[p->target_id]._sl.lock();
				clients[p->target_id]._s_state = ST_FREE;
				clients[p->target_id]._sl.unlock();
			}
		}
		else {						// 공격자가 npc
			clients[c_id].hp -= 10;
			Update_Player(c_id);
		}
		break;
	}
	case CS_COLOR: {
		CS_COLOR_PACKET* p = reinterpret_cast<CS_COLOR_PACKET*>(packet);
		clients[c_id].color = p->color;
		break;
	}
	case SS_CONNECT_SERVER: {
		cout << "SS_CONNECT_SERVER 로비 서버 커넥트" << endl;
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
		clients[c_id].x = 0;
		clients[c_id].y = 0;
		clients[c_id].z = 0;
		clients[c_id].degree = 0;
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._Room_Num = 999;
		clients[c_id].room_list.clear();
		clients[c_id].char_state = 4;
		clients[c_id]._sl.unlock();
		

		SS_CONNECT_SERVER_PACKET pi;
		pi.size = sizeof(SS_CONNECT_SERVER_PACKET);
		pi.type = SS_CONNECT_SERVER;
		strcpy_s(pi.name, "game");
		
		clients[c_id].do_send(&pi);
		break;
	}
	case SS_DATA_PASS: {
		cout << "서버 간 데이터 송신 들어옴" << endl;
		SS_DATA_PASS_PACKET* p = reinterpret_cast<SS_DATA_PASS_PACKET*>(packet);

		cout << p->name << endl;
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

	if (clients[c_id].room_list.size() != 0) {
		for (auto& pl : clients[c_id].room_list) {
			if (pl > MAX_USER) {
				clients[pl].room_list.erase(c_id);
				continue;
			}
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
	clients[c_id]._sl.unlock();
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
				clients[client_id].z = 0;
				clients[client_id].degree = 0;
				clients[client_id].char_state = 0;
				clients[client_id].hp = 100;
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
			//cout << "npc : " << key << ", player : " << ex_over->target_id << endl;
			move_npc(ex_over->target_id, key);
			delete ex_over;
			break;
		}
		case OP_NPC_RUSH: {
			float t_x = clients[key].target_x;
			float t_z = clients[key].target_z;
			//cout << key << endl;
			rush_npc(key, t_x, t_z);
			delete ex_over;
			break;
		}
		case OP_CREATE_CUBE: {
			send_cube(ex_over->target_id, clients[ex_over->target_id].x, clients[ex_over->target_id].y, clients[ex_over->target_id].z);
			delete ex_over;
			break;
		}
		case OP_UPDATE: {
			Update_Npc();
			add_timer(0, 33, EV_UP, 0);
			delete ex_over;
			break;
		}
		case OP_NPC_RETURN: {
			return_npc(key);
			delete ex_over;
			break;
		}
		case OP_SET_NPC: {
			close_lua(key);
			delete ex_over;
			break;
		}
		}
	}
}

void Update_Player(int c_id)
{
	clients[c_id]._sl.lock();
	if (clients[c_id]._s_state != ST_INGAME) {
		clients[c_id]._sl.unlock();
		return;
	}
	clients[c_id]._sl.unlock();
	
	clients[c_id].send_move_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree,
		clients[c_id].hp, clients[c_id].char_state, clients[c_id].client_time);

	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		clients[pl].send_move_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree,
			clients[c_id].hp, clients[c_id].char_state, 0);
	}
	
}

void Update_Npc()
{
	for (int i = 0; i < MAX_USER; i++) {
		clients[i]._sl.lock();
		if (clients[i]._s_state != ST_INGAME) {
			clients[i]._sl.unlock();
			continue;
		}
		clients[i]._sl.unlock();

		for (auto& pl : clients[i].room_list) {
			if (pl < MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			clients[i].send_move_packet(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
				clients[pl].hp, clients[pl].char_state, 0);
		}
	}
}