
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
extern array<KEY, KEY_NUM> keys;
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
		clients[c_id].stage = p->stage;
		switch (clients[c_id].stage)
		{
		case 0: {
			clients[c_id].x = 0;
			clients[c_id].y = 0;
			clients[c_id].z = 0;
			break;
		}
		case 1: {
			clients[c_id].x = 170;
			clients[c_id].y = 0;
			clients[c_id].z = -230;
			
			break;
		}
		default:
			break;
		}
		clients[c_id].degree = 30;
		clients[c_id].char_state = AN_IDLE;
		clients[c_id].color = 0;
		clients[c_id].god_mod = false;
		clients[c_id].hp = 100;
		clients[c_id].atk = 100;
		clients[c_id].skill_atk = 500;
		clients[c_id].send_login_ok_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp);
		clients[c_id]._s_state = ST_INGAME;
		if (clients[0]._Room_Num != 999) clients[c_id]._Room_Num = c_id / ROOM_USER;
		else clients[c_id]._Room_Num = (c_id - 1) / ROOM_USER;
		clients[c_id].room_list.clear();
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
					clients[c_id]._name, clients[c_id].hp, clients[c_id].char_state, clients[i]._object_type);
				clients[c_id].send_add_object(pl._id, pl.x, pl.y, pl.z, pl.degree, pl._name, pl.hp, pl.char_state, clients[i]._object_type);
				pl.room_list.insert(c_id);
				clients[c_id].room_list.insert(pl._id);
			}
			pl._sl.unlock();
		}

		// npc 세팅 부분
		if (clients[c_id]._Room_Num != 999) {
			if (clients[c_id].room_list.size() == 0 && clients[clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC - 1].Lua_on == false) {
				for (int i = clients[c_id]._Room_Num * ROOM_NPC + MAX_USER; i < clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC; i++) {
					switch (clients[c_id].stage)
					{
					case 0: // 테스트 스테이지
						break;
					case 1: {// 스테이지 1

						if (i == clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC - 1) {
							clients[i]._object_type = TY_BOSS_1;
						}

						if ((i - MAX_USER) % ROOM_NPC < 3) {
							clients[i].x = 130;
							clients[i].start_x = 130;
							clients[i].z = -240 + (i - MAX_USER) % ROOM_NPC * 10;
							clients[i].start_z = -240 + (i - MAX_USER) % ROOM_NPC * 10;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 2 && (i - MAX_USER) % ROOM_NPC < 6) {
							clients[i].x = 100;
							clients[i].start_x = 100;
							clients[i].z = -270 + (i - MAX_USER) % ROOM_NPC * 10;
							clients[i].start_z = -270 + (i - MAX_USER) % ROOM_NPC * 10;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 5 && (i - MAX_USER) % ROOM_NPC < 9) {
							clients[i].x = 70;
							clients[i].start_x = 70;
							clients[i].z = -300 + (i - MAX_USER) % ROOM_NPC * 20;
							clients[i].start_z = -300 + (i - MAX_USER) % ROOM_NPC * 20;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 8 && (i - MAX_USER) % ROOM_NPC < 12) {
							clients[i].x = 0;
							clients[i].start_x = 0;
							clients[i].z = -80 - (i - MAX_USER - 8) % ROOM_NPC * 20;
							clients[i].start_z = -80 - (i - MAX_USER - 8) % ROOM_NPC * 20;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 11 && (i - MAX_USER) % ROOM_NPC < 15) {
							clients[i].x = 20;
							clients[i].start_x = 20;
							clients[i].z = -80 - (i - MAX_USER - 11) % ROOM_NPC * 20;
							clients[i].start_z = -80 - (i - MAX_USER - 11) % ROOM_NPC * 20;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 14 && (i - MAX_USER) % ROOM_NPC < 18) {
							clients[i].x = 35;
							clients[i].start_x = 35;
							clients[i].z = -80 - (i - MAX_USER - 14) % ROOM_NPC * 20;
							clients[i].start_z = -80 - (i - MAX_USER - 14) % ROOM_NPC * 20;
						}
						else if ((i - MAX_USER) % ROOM_NPC > 17 && (i - MAX_USER) % ROOM_NPC < 19) {
							clients[i].x = 0;
							clients[i].start_x = 0;
							clients[i].z = 0;
							clients[i].start_z = 0;
						}
						else {
							clients[i].x = -340;
							clients[i].start_x = -340;
							clients[i].z = -145;
							clients[i].start_z = -145;
						}
						break;
					}
					case 2: {
						if (i == clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC - 1) {
							clients[i]._object_type = TY_BOSS_2;
						}

						break;
					}
					default:
						break;
					}

					if (i % ROOM_NPC != ROOM_NPC - 1) {
						add_timer(i, 10000, EV_NPC_CON, c_id);
						clients[i].y = 0.3f;
					}
					else {
						add_timer(i, 10000, EV_BOSS_CON, c_id);
					}
					clients[i].Lua_on = true;
				}

				for (int i = clients[c_id]._Room_Num * ROOM_KEY; i < clients[c_id]._Room_Num * ROOM_KEY + ROOM_KEY; i++) {
					clients[c_id].send_key(i, keys[i].x, keys[i].y, keys[i].z, keys[i].color);
				}
			}
		}

		for (int i = MAX_USER; i < MAX_USER + NPC_NUM; i++) {
			if (clients[c_id]._Room_Num == clients[i]._Room_Num) {
				clients[c_id].room_list.insert(i);
				clients[i].room_list.insert(c_id);
				if (i % ROOM_NPC != ROOM_NPC - 1)
					clients[c_id].send_add_object(i, clients[i].x, clients[i].y, clients[i].z, clients[i].degree, clients[i]._name, clients[i].hp, clients[i].char_state, clients[i]._object_type);
				else {
					clients[c_id].send_boss_add(i, clients[i].x, clients[i].y, clients[i].z, clients[i].degree, clients[i]._name, clients[i].hp, clients[i].char_state);
				}
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

		Update_Player(c_id);
		break;
	}
	case CS_CONSOLE: {
		reset_lua(c_id);
		break;
	}
	case CS_COLLISION: {
		CS_COLLISION_PACKET* p = reinterpret_cast<CS_COLLISION_PACKET*>(packet);
		if (p->attacker_id < MAX_USER) {	// 공격자가 플레이어
			if (p->attack_type == 2) { // 일반 공격
				if (clients[p->target_id].boss_shield_trigger == true) { // 보스 기믹 중
					clients[p->target_id].boss_shield -= clients[p->attacker_id].atk;
					cout << "보스 쉴드 파괴 중" << endl;
					if (clients[p->target_id].boss_shield <= 0) {
						clients[p->target_id].crash_sequence[3] = 0;
						cout << "보스 쉴드 파괴 : 0" << endl;
					}
				}

				else { // 일반
					clients[p->target_id].hp -= clients[p->attacker_id].atk;
					if (clients[p->target_id].hp <= 0) {
						clients[p->target_id].char_state = AN_DEAD;
						clients[p->target_id]._sl.lock();
						clients[p->target_id]._s_state = ST_FREE;
						clients[p->target_id]._sl.unlock();
					}
				}
			}
			else { // 스킬 공격
				if (p->attack_type == 1) { // 스킬 공격
					if (clients[p->target_id].boss_shield_trigger == true) { // 보스 기믹 중
						clients[p->target_id].boss_shield -= clients[p->attacker_id].skill_atk;
						cout << "보스 쉴드 파괴 중" << endl;
						if (clients[p->target_id].boss_shield <= 0) {
							clients[p->target_id].crash_sequence[3] = 0;
							cout << "보스 쉴드 파괴 : 0" << endl;
						}
					}

					else { // 일반
						clients[p->target_id].hp -= clients[p->attacker_id].skill_atk;
						if (clients[p->target_id].hp <= 0) {
							clients[p->target_id].char_state = AN_DEAD;
							clients[p->target_id]._sl.lock();
							clients[p->target_id]._s_state = ST_FREE;
							clients[p->target_id]._sl.unlock();
						}
					}
				}
			}

			int pl_id = clients[p->attacker_id]._Room_Num * ROOM_USER;
			int pl = p->target_id;

			if (clients[pl]._object_type == TY_BOSS_1 ||
				clients[pl]._object_type == TY_BOSS_2 ||
				clients[pl]._object_type == TY_BOSS_3) {
				for (int i = pl_id; i < pl_id + ROOM_USER; i++) {
					clients[pl_id]._sl.lock();
					if (clients[pl_id]._s_state != ST_INGAME) {
						clients[pl_id]._sl.unlock();
						continue;
					}
					
					clients[i].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
						clients[pl].hp, clients[pl].char_state, clients[pl].eye_color, 0);
					clients[pl_id]._sl.unlock();
				}
			}
			else {
				for (int i = pl_id; i < pl_id + ROOM_USER; i++) {
					clients[pl_id]._sl.lock();
					if (clients[pl_id]._s_state != ST_INGAME) {
						clients[pl_id]._sl.unlock();
						continue;
					}

					clients[i].send_move_packet(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
						clients[pl].hp, clients[pl].char_state, 0);
					clients[pl_id]._sl.unlock();
				}
			}
		}
		else {						// 공격자가 npc
			if (clients[c_id].god_mod == true) break;
			else {
				clients[c_id].hp -= clients[p->attacker_id].atk;
			}
			Update_Player(c_id);
		}
		break;
	}
	case CS_KEY: {
		int cnt = 0;
		CS_KEY_PACKET* p = reinterpret_cast<CS_KEY_PACKET*>(packet);
		clients[c_id].color = p->color;
		keys[p->key_id].on_field = false;

		for (int i = clients[c_id]._Room_Num * ROOM_KEY; i < clients[c_id]._Room_Num * 4 + ROOM_KEY; ++i) {
			if (keys[i].on_field == true) break;
			else cnt++;
		}

		if (cnt == ROOM_KEY) {

			for (int i = clients[c_id]._Room_Num * ROOM_USER; i < clients[c_id]._Room_Num * 4 + ROOM_USER; ++i) {
				clients[i].send_door();
			}
		}
		break;
	}
	case CS_OBJECT_COLLISION: {
		CS_OBJECT_COLLISION_PACKET* p = reinterpret_cast<CS_OBJECT_COLLISION_PACKET*>(packet);
		int boss_num = clients[c_id]._Room_Num * ROOM_NPC + ROOM_NPC - 1 + MAX_USER;
		switch (p->object_type)
		{
		case 0: // 1스테이지 큐브, 기둥
			clients[boss_num].crash_sequence[clients[boss_num].crash_count] = cubes[p->target_id].color;
			clients[boss_num].crash_count++;
			break;
		case 1: // 2스테이지 장판
			break;
		default:
			break;
		}
		break;
	}
	case CS_GOD_MOD: {
		if (clients[c_id].god_mod == false) {
			clients[c_id].atk *= 100;
			clients[c_id].skill_atk *= 100;
			clients[c_id].god_mod = true;
			cout << "갓모드 on" << endl;
		}
		else {
			clients[c_id].atk /= 100;
			clients[c_id].skill_atk /= 100;
			clients[c_id].god_mod = false;
			cout << "갓모드 off" << endl;
		}
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
		clients[c_id].char_state = AN_DEAD;
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
	cout << "disconnect" << endl;
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
				clients[client_id].char_state = AN_IDLE;
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
			move_npc(ex_over->target_id, key);
			delete ex_over;
			break;
		}
		case OP_NPC_RUSH: {
			float t_x = clients[key].target_x;
			float t_z = clients[key].target_z;
			rush_npc(key, t_x, t_z);
			delete ex_over;
			break;
		}
		case OP_CREATE_CUBE: {
			send_cube(ex_over->target_id, clients[ex_over->target_id].x, clients[ex_over->target_id].y, clients[ex_over->target_id].z);
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
		case OP_BOSS_WANDER: {
			wander_boss(key);
			delete ex_over;
			break;
		}
		case OP_NPC_CON: {
			if (abs(clients[key].x - clients[ex_over->target_id].x) + abs(clients[key].z - clients[ex_over->target_id].z) <= 15) {
				move_npc(ex_over->target_id, key);
			}
			else {
				if (clients[key].start_x != clients[key].x || clients[key].start_z != clients[key].z) {
					return_npc(key);
				}
			}
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