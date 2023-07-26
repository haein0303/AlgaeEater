#pragma once

#include "NPC.h"
#include "stdafx.h"
#include "Session.h"
#include "Timer.h"
#include "util.h"
#include "Lua_API.h"


extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;
//extern array<KEY, KEY_NUM> keys;
extern array<FIELD, FIELD_NUM> fields;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

void initialize_npc()
{
	for (int i = MAX_USER; i < MAX_USER + NPC_NUM; ++i) {
		clients[i]._s_state = ST_INGAME;
		clients[i]._Room_Num = (i - MAX_USER) / ROOM_NPC;
		clients[i].y = 0;
		clients[i].degree = 0;
		clients[i].start_x = 0;
		clients[i].start_z = 0;
		clients[i].atk = 1;
		clients[i].char_state = AN_IDLE;
		clients[i]._name[0] = 0;
		clients[i].god_mod = false;
		clients[i]._prev_remain = 0;
		clients[i].Lua_on = false;
		clients[i].stage = 0;
		clients[i].turn = 0;
		clients[i].boss_shield = 100;
		clients[i].first_pattern = false;
		clients[i].second_pattern = false;
		clients[i].boss_shield_trigger = false;
		clients[i].room_list.clear();
		clients[i].object_list.clear();

		for (int j = 0; j < CUBE_NUM; j++) {
			if (cubes[j]._Room_Num != clients[i]._Room_Num) continue;
			else clients[i].object_list.insert(j);
		}

		int st = (i - MAX_USER - clients[i]._Room_Num * ROOM_NPC);

		clients[i]._object_type = TY_MOVE_NPC;

		switch (st)
		{
		case 40:
		case 30:
		case 20:
		case 10:
		case 0:
			srand((unsigned int)time(NULL));
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = -20;
			clients[i].z = -20;
			break;
		case 41:
		case 31:
		case 21:
		case 11:
		case 1:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = -20;
			clients[i].z = 0;
			break;
		case 42:
		case 32:
		case 22:
		case 12:
		case 2:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = -20;
			clients[i].z = 20;
			break;
		case 43:
		case 33:
		case 23:
		case 13:
		case 3:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 0;
			clients[i].z = -20;
			break;
		case 44:
		case 34:
		case 24:
		case 14:
		case 4:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 0;
			clients[i].z = 0;
			break;
		case 45:
		case 35:
		case 25:
		case 15:
		case 5:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 0;
			clients[i].z = 20;
			break;
		case 46:
		case 36:
		case 26:
		case 16:
		case 6:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 20;
			clients[i].z = -20;
			break;
		case 47:
		case 37:
		case 27:
		case 17:
		case 7:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 20;
			clients[i].z = 0;
			break;
		case 48:
		case 38:
		case 28:
		case 18:
		case 8:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 20;
			clients[i].z = 20;
			break;
		case 39:
		case 29:
		case 19:
		case 9:
			clients[i].hp = (rand() % 3 + 5) * 10;
			clients[i].x = 20;
			clients[i].z = 20;
			break;
		case 49: // 보스
		{
			clients[i].hp = BOSS_HP[0];
			clients[i].atk = 2;
			clients[i].x = 30;
			clients[i].z = 30;
			clients[i].eye_color = 0;
			clients[i]._object_type = TY_BOSS_1;

			clients[i].L = luaL_newstate();

			luaL_openlibs(clients[i].L);

			luaL_loadfile(clients[i].L, "hello.lua");
			lua_pcall(clients[i].L, 0, 0, 0);

			lua_getglobal(clients[i].L, "set_object_id");
			lua_pushnumber(clients[i].L, i);
			lua_pcall(clients[i].L, 1, 0, 0);

			lua_register(clients[i].L, "API_get_x", API_get_x);
			lua_register(clients[i].L, "API_get_z", API_get_z);
			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);

			lua_register(clients[i].L, "API_Wander", API_Wander);
			lua_register(clients[i].L, "API_Rush", API_Rush);
			lua_register(clients[i].L, "API_get_state", API_get_state);
			break;
		}
		default:
			break;
		}

		clients[i].start_x = clients[i].x;
		clients[i].start_z = clients[i].z;
	}

	for (int i = MAX_USER; i < MAX_USER + NPC_NUM; ++i) {
		for (int j = i + 1; j < i + ROOM_NPC; ++j) {
			if (j >= MAX_USER + NPC_NUM) continue;
			if (clients[i]._Room_Num != clients[j]._Room_Num) continue;
			else
			{
				clients[i].room_list.insert(j);
				clients[j].room_list.insert(i);
			};
		}
	}
	cout << "npc 로딩 끝" << endl;
}

void initialize_cube()
{
	for (int i = 0; i < CUBE_NUM; i++) {
		cubes[i].x = 10.0f;
		cubes[i].y = 0;
		cubes[i].z = 30.0f;
		cubes[i].degree = 0;
		cubes[i].color = 0;
		cubes[i].hp = 10;
		cubes[i]._Room_Num = i / ROOM_CUBE;
	}
	cout << "cube 로딩 끝" << endl;
}

//void initialize_key()
//{
//	for (int i = 0; i < KEY_NUM; i++) {
//		keys[i].x = 165.f;
//		keys[i].y = 0.f;
//		keys[i].z = -225.f + i * 5;
//		keys[i].color = i % KEY_NUM;
//		keys[i]._Room_Num = i / ROOM_KEY;
//		keys[i].on_field = true;
//	}
//	cout << "key 로딩 끝" << endl;
//}

void initialize_field()
{
	for (int i = 0; i < FIELD_NUM; i++) {
		fields[i].x = 0.f;
		fields[i].r = 3.f;
		fields[i].z = 0.f;
		fields[i].type = FD_REC;
		fields[i]._Room_Num = i / ROOM_FIELD;
	}
	cout << "field 로딩 끝" << endl;
}

void send_cube(int c_id, float x, float y, float z)
{
	int s_num = clients[c_id]._Room_Num * ROOM_CUBE;
	int cnt = 0;

	for (int i = s_num; i < s_num + ROOM_CUBE - 1; i++) {
		switch (cnt)
		{
		case 0:
			cubes[i].x = x;
			cubes[i].y = y;
			cubes[i].z = z - 10.0f;
			cubes[i].degree = 0;
			cubes[i].color = cnt + 1;
			break;
		case 1:
			cubes[i].x = x - 10.0f;
			cubes[i].y = y;
			cubes[i].z = z;
			cubes[i].degree = 0;
			cubes[i].color = cnt + 1;
			break;
		case 2:
			cubes[i].x = x;
			cubes[i].y = y;
			cubes[i].z = z + 10.0f;
			cubes[i].degree = 0;
			cubes[i].color = cnt + 1;
			break;
		}
		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			clients[pl].send_cube_add(i % ROOM_CUBE, cubes[i].x, cubes[i].y, cubes[i].z, cubes[i].degree, cubes[i].color, cubes[i].hp);
		}

		cnt++;
	}
}

void rush_npc(int c_id, float t_x, float t_z)
{
	float x = clients[c_id].x;
	float z = clients[c_id].z;
	int tar_id = 0;

	float de = atan2(x - t_x, z - t_z);
	float nde = de * 180 / PI;
	clients[c_id].degree = nde;

	// 큐브랑 충돌 못했음 -> 기믹 실패
	if (x == t_x && z == t_z) {
		cout << "기믹 실패" << endl;

		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			clients[pl].hp /= 2;

			Update_Player(pl);
		}

		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME || clients[pl].char_state == AN_DEAD) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			tar_id = pl;
			break;
		}

		SC_BOSS_RUSH_TARGET_PACKET p;
		p.size = sizeof(SC_BOSS_RUSH_TARGET_PACKET);
		p.type = SC_BOSS_RUSH_TARGET;
		p.target_id = tar_id;
		p.trigger = true;

		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;

			clients[pl].do_send(&p);
		}

		add_timer(c_id, 10000, EV_STAGE1_SECOND_BOSS, tar_id);
		return;
	}

	// 큐브랑 충돌 함 -> 기믹 성공
	int cube_num = clients[c_id]._Room_Num * ROOM_CUBE + ROOM_CUBE - 1;
	if (abs(x - cubes[cube_num].x) + abs(z - cubes[cube_num].z) <= 4) {
		// 반복 설정은 여기서 해줘야 함
		// 타겟 아이디 설정 해줘야 함 ㅇㅇ
		cout << "기믹 성공" << endl;
		clients[c_id].char_state = AN_DOWN;

		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			int cli_cube_num = cube_num % ROOM_CUBE;

			clients[pl].send_remove_object(cli_cube_num, 1);

			clients[pl].send_boss_move(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, 0, 0);
		}

		for (auto& pl : clients[c_id].room_list) {
			if (pl >= MAX_USER) continue;
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME || clients[pl].char_state == AN_DEAD) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			tar_id = pl;
			break;
		}

		add_timer(c_id, 10000, EV_BOSS_CON, tar_id);
		return;
	}

	x += 0.5f * -sin(de);
	z += 0.5f * -cos(de);

	clients[c_id].x = x;
	clients[c_id].z = z;

	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		clients[pl].send_boss_move(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, clients[c_id].eye_color, 0);
	}

	auto ex_over = new OVER_EXP;
	ex_over->_comp_type = OP_NPC_RUSH;
	ex_over->target_id = c_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, c_id, &ex_over->_over);
}

void move_npc(int player_id, int c_id)
{
	float x = clients[c_id].x;
	float z = clients[c_id].z;

	float de = atan2(x - clients[player_id].x, z - clients[player_id].z);
	float nde = de * 180 / PI;

	if (clients[c_id]._object_type == TY_MOVE_NPC) {
		if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 1.5f) {
			// 공격 처리 관련, 여기서 안 할 수도 있음
			clients[c_id].char_state = AN_ATTACK_1;
			clients[c_id].degree = nde;

			for (auto& pl : clients[c_id].room_list) {
				if (pl >= MAX_USER) continue;
				clients[pl]._sl.lock();
				if (clients[pl]._s_state != ST_INGAME) {
					clients[pl]._sl.unlock();
					continue;
				}
				clients[pl]._sl.unlock();

				clients[pl].send_move_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, 0);
			}

			return;
		}
		else clients[c_id].char_state = AN_WALK;
	}
	else {
		if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 5.f) {
			// 공격 처리 관련, 여기서 안 할 수도 있음
			clients[c_id].char_state = AN_ATTACK_1;
			clients[c_id].degree = nde;

			for (auto& pl : clients[c_id].room_list) {
				if (pl >= MAX_USER) continue;
				clients[pl]._sl.lock();
				if (clients[pl]._s_state != ST_INGAME) {
					clients[pl]._sl.unlock();
					continue;
				}
				clients[pl]._sl.unlock();

				clients[pl].send_boss_move(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, clients[c_id].eye_color, 0);
			}
			return;
		}
		else clients[c_id].char_state = AN_WALK;
	}

	x += 0.3f * -sin(de);
	z += 0.3f * -cos(de);

	int tic = 0;

	for (auto& pl : clients[c_id].object_list) {
		if (abs(x - cubes[pl].x) + abs(z - cubes[pl].z) <= 2.5f) {
			int f = 0;
			int d = 0;
			x = clients[c_id].x;
			z = clients[c_id].z;
			float f_nde = nde + 90;
			float d_nde = nde - 90;

			float f_de = f_nde / 180 * PI;
			float d_de = d_nde / 180 * PI;

			float f_x = x + 0.3f * -sin(f_de);
			float f_z = z + 0.3f * -cos(f_de);

			float d_x = x + 0.3f * -sin(d_de);
			float d_z = z + 0.3f * -cos(d_de);

			if (abs(f_x - cubes[pl].x) + abs(f_z - cubes[pl].z) <= 2.5f) f = 1;
			if (abs(d_x - cubes[pl].x) + abs(d_z - cubes[pl].z) <= 2.5f) d = 1;

			if (f == 1 && d == 1) {
				int f = 0;
				int d = 0;

				float f_nde = nde + 120;
				float d_nde = nde - 120;

				float f_de = f_nde / 180 * PI;
				float d_de = d_nde / 180 * PI;

				float f_x = x + 0.3f * -sin(f_de);
				float f_z = z + 0.3f * -cos(f_de);

				float d_x = x + 0.3f * -sin(d_de);
				float d_z = z + 0.3f * -cos(d_de);

				if (abs(f_x - cubes[pl].x) + abs(f_z - cubes[pl].z) <= 2.5f) f = 1;
				if (abs(d_x - cubes[pl].x) + abs(d_z - cubes[pl].z) <= 2.5f) d = 1;
			}

			if (f == 1 && d == 0) {
				x = d_x;
				z = d_z;
				de = d_de;
				nde = d_nde;
			}
			else if (f == 0 && d == 1) {
				x = f_x;
				z = f_z;
				de = f_de;
				nde = f_nde;
			}
			else if (f == 0 && d == 0) {
				if (clients[c_id].turn == 0) {
					float fabs = abs(f_x - clients[player_id].x) + abs(f_z - clients[player_id].z);
					float dabs = abs(d_x - clients[player_id].x) + abs(d_z - clients[player_id].z);

					if (fabs >= dabs) clients[c_id].turn = 1;
					else clients[c_id].turn = -1;
				}
				if (clients[c_id].turn == -1) {
					x = d_x;
					z = d_z;
					de = d_de;
					nde = d_nde;
				}
				else {
					x = f_x;
					z = f_z;
					de = f_de;
					nde = f_nde;
				}
			}
		}
		else tic++;
	}

	for (auto& pl : clients[c_id].room_list) {
		if (pl == player_id) continue;
		if (abs(x - clients[pl].x) + abs(z - clients[pl].z) <= 1.5f) {
			int f = 0;
			int d = 0;
			x = clients[c_id].x;
			z = clients[c_id].z;
			float f_nde = nde + 90;
			float d_nde = nde - 90;

			float f_de = f_nde / 180 * PI;
			float d_de = d_nde / 180 * PI;

			float f_x = x + 0.3f * -sin(f_de);
			float f_z = z + 0.3f * -cos(f_de);

			float d_x = x + 0.3f * -sin(d_de);
			float d_z = z + 0.3f * -cos(d_de);

			if (abs(f_x - clients[pl].x) + abs(f_z - clients[pl].z) <= 1.5f) f = 1;
			if (abs(d_x - clients[pl].x) + abs(d_z - clients[pl].z) <= 1.5f) d = 1;

			if (f == 1 && d == 1) {
				int f = 0;
				int d = 0;

				float f_nde = nde + 120;
				float d_nde = nde - 120;

				float f_de = f_nde / 180 * PI;
				float d_de = d_nde / 180 * PI;

				float f_x = x + 0.3f * -sin(f_de);
				float f_z = z + 0.3f * -cos(f_de);

				float d_x = x + 0.3f * -sin(d_de);
				float d_z = z + 0.3f * -cos(d_de);

				if (abs(f_x - clients[pl].x) + abs(f_z - clients[pl].z) <= 1.5f) f = 1;
				if (abs(d_x - clients[pl].x) + abs(d_z - clients[pl].z) <= 1.5f) d = 1;
			}

			if (f == 1 && d == 0) {
				x = d_x;
				z = d_z;
				de = d_de;
				nde = d_nde;
			}
			else if (f == 0 && d == 1) {
				x = f_x;
				z = f_z;
				de = f_de;
				nde = f_nde;
			}
			else if (f == 0 && d == 0) {
				if (clients[c_id].turn == 0) {
					float fabs = abs(f_x - clients[player_id].x) + abs(f_z - clients[player_id].z);
					float dabs = abs(d_x - clients[player_id].x) + abs(d_z - clients[player_id].z);

					if (fabs >= dabs) clients[c_id].turn = 1;
					else clients[c_id].turn = -1;
				}
				if (clients[c_id].turn == -1) {
					x = d_x;
					z = d_z;
					de = d_de;
					nde = d_nde;
				}
				else {
					x = f_x;
					z = f_z;
					de = f_de;
					nde = f_nde;
				}
			}
		}
		else tic++;
	}

	if (tic == clients[c_id].room_list.size() + clients[c_id].object_list.size()) clients[c_id].turn = 0;

	clients[c_id].x = x;
	clients[c_id].z = z;
	clients[c_id].degree = nde;

	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		if (ROOM_NPC - 1 == c_id % ROOM_NPC)
			clients[pl].send_boss_move(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, clients[c_id].eye_color, 0);
		else
			clients[pl].send_move_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, 0);
	}
}

void return_npc(int c_id)
{
	float x = clients[c_id].x;
	float z = clients[c_id].z;
	clients[c_id].char_state = AN_WALK;

	float de = atan2(x - clients[c_id].start_x, z - clients[c_id].start_z);
	float nde = de * 180 / PI;
	clients[c_id].degree = nde;

	x += 0.5f * -sin(de);
	z += 0.5f * -cos(de);

	if (abs(x - clients[c_id].start_x) < 0.5f) x = clients[c_id].start_x;
	if (abs(z - clients[c_id].start_z) < 0.5f) z = clients[c_id].start_z;

	if (x == clients[c_id].start_x && z == clients[c_id].start_z) clients[c_id].char_state = AN_IDLE;

	clients[c_id].x = x;
	clients[c_id].z = z;

	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		clients[pl].send_move_packet(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, 0);
	}
}

void wander_boss(int c_id)
{
	if (clients[c_id].stack == 10) {
		clients[c_id].stack = 0;

		srand((unsigned int)time(NULL));

		clients[c_id].degree = rand() % 360;
	}
	
	float x = clients[c_id].x;
	float z = clients[c_id].z;
	clients[c_id].char_state = AN_WALK;

	if (abs(x - clients[c_id].start_x) + abs(z - clients[c_id].start_z) > 30) {
		float e = atan2(x - clients[c_id].start_x, z - clients[c_id].start_z);
		float ne = e * 180 / PI;
		clients[c_id].degree = ne;
		clients[c_id].stack = -10;
	}
	float de = clients[c_id].degree / 180 * PI;

	x += 0.3f * -sin(de);
	z += 0.3f * -cos(de);

	clients[c_id].stack++;

	clients[c_id].x = x;
	clients[c_id].z = z;

	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		clients[pl].send_boss_move(c_id, clients[c_id].x, clients[c_id].y, clients[c_id].z, clients[c_id].degree, clients[c_id].hp, clients[c_id].char_state, clients[c_id].eye_color, 0);
	}
}

void send_second_cube(int c_id, float x, float y, float z)
{
	int s_num = clients[c_id]._Room_Num * ROOM_CUBE + ROOM_CUBE - 1;

	cubes[s_num].x = x;
	cubes[s_num].y = y;
	cubes[s_num].z = z - 10.0f;
	cubes[s_num].degree = 0;
	cubes[s_num].color = 0;
		
	for (auto& pl : clients[c_id].room_list) {
		if (pl >= MAX_USER) continue;
		clients[pl]._sl.lock();
		if (clients[pl]._s_state != ST_INGAME) {
			clients[pl]._sl.unlock();
			continue;
		}
		clients[pl]._sl.unlock();

		clients[pl].send_cube_add(s_num % ROOM_CUBE, cubes[s_num].x, cubes[s_num].y, cubes[s_num].z, cubes[s_num].degree, cubes[s_num].color, cubes[s_num].hp);
	}
}
