#pragma once

#include "NPC.h"
#include "stdafx.h"
#include "Session.h"
#include "Timer.h"
#include "Lua_API.h"


extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

void initialize_npc()
{
	for (int i = MAX_USER; i < MAX_USER + NPC_NUM; ++i) {
		clients[i]._s_state = ST_INGAME;
		clients[i].x = (i - MAX_USER) / 10;
		clients[i].y = 0;
		clients[i].z = 0;
		clients[i].degree = 0;
		clients[i].move_stack = 0;
		clients[i].move_degree = 0;
		clients[i]._name[0] = 0;
		clients[i]._prev_remain = 0;
		clients[i]._Room_Num = (i - MAX_USER) / 10;

		if (i % 10 == 9)
		{
			clients[i]._s_state = ST_INGAME;
			clients[i].x = (i - MAX_USER) / 10;
			clients[i].y = 0;
			clients[i].z = 0;
			clients[i].degree = 0;
			clients[i].move_stack = 0;
			clients[i].move_degree = 0;
			clients[i]._name[0] = 0;
			clients[i]._prev_remain = 0;
			clients[i]._Room_Num = (i - MAX_USER) / 10;

			clients[i].L = luaL_newstate();

			luaL_openlibs(clients[i].L);

			if (luaL_loadfile(clients[i].L, "hello.lua")) {
				//printf("소환 실패");
			}
			else {
				//printf("소환 성공");
			}
			lua_pcall(clients[i].L, 0, 0, 0);

			lua_getglobal(clients[i].L, "set_object_id");
			lua_pushnumber(clients[i].L, i);
			lua_pcall(clients[i].L, 1, 0, 0);

			lua_register(clients[i].L, "API_get_x", API_get_x);
			lua_register(clients[i].L, "API_get_z", API_get_z);
			lua_register(clients[i].L, "API_Rush", API_Rush);
			lua_register(clients[i].L, "API_Cube", API_Cube);
			lua_register(clients[i].L, "API_get_state", API_get_state);
		}
	}
	cout << "npc 로딩 끝" << endl;
}

void initialize_cube()
{
	for (int i = 0; i < CUBE_NUM; i++) {
		cubes[i].x = 0;
		cubes[i].y = 0;
		cubes[i].z = 0;
		cubes[i].degree = 0;
		cubes[i]._Room_Num = i / 4;
	}
	cout << "cube 로딩 끝" << endl;
}

void send_cube(int c_id, float x, float y, float z)
{
	int s_num = clients[c_id]._Room_Num * 4;
	int cnt = 0;

	for (int i = s_num; i < s_num + 4; i++) {
		switch (cnt)
		{
		case 0:
			cubes[i].x = x;
			cubes[i].y = y;
			cubes[i].z = z - 10.0f;
			cubes[i].degree = 0;
			break;
		case 1:
			cubes[i].x = x - 10.0f;
			cubes[i].y = y;
			cubes[i].z = z;
			cubes[i].degree = 0;
			break;
		case 2:
			cubes[i].x = x;
			cubes[i].y = y;
			cubes[i].z = z + 10.0f;
			cubes[i].degree = 0;
			break;
		case 3:
			cubes[i].x = x + 10.0f;
			cubes[i].y = y;
			cubes[i].z = z;
			cubes[i].degree = 0;
			break;
		}

		for (auto& pl : clients[c_id].room_list) {
			clients[pl]._sl.lock();
			if (clients[pl]._s_state != ST_INGAME) {
				clients[pl]._sl.unlock();
				continue;
			}
			clients[pl]._sl.unlock();

			if (pl < 400) {
				clients[pl].send_cube_add(i % 4, cubes[i].x, cubes[i].y, cubes[i].z, cubes[i].degree);
			}
		}

		cnt++;
	}

	add_timer(0, 5000, EV_CB, c_id);
}

void rush_npc(int player_id, int c_id)
{
	float x = clients[c_id].x;
	float z = clients[c_id].z;

	Sleep(33);

	if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 2) {
		add_timer(c_id, 10000, EV_RUSH, player_id);
		return;
	}

	for (int i = clients[c_id]._Room_Num * 4; i < clients[c_id]._Room_Num * 4 + 4; i++) {
		if (abs(x - cubes[i].x) + abs(z - cubes[i].z) <= 2) {
			cout << "기둥 충돌" << endl;
			add_timer(c_id, 10000, EV_RUSH, player_id);
			return;
		}
	}

	float de = atan2(x - clients[player_id].x, z - clients[player_id].z);
	de = de * 180 / PI;
	clients[c_id].degree = de;

	if (x > clients[player_id].x) x--;
	else if (x < clients[player_id].x) x++;

	if (z > clients[player_id].z) z--;
	else if (z < clients[player_id].z) z++;

	if (abs(x - clients[player_id].x) < 1) x = clients[player_id].x;
	if (abs(z - clients[player_id].z) < 1) z = clients[player_id].z;

	clients[c_id].x = x;
	clients[c_id].z = z;

	rush_npc(player_id, c_id);
}

void move_npc(int npc_id)
{
	float z = clients[npc_id].z;
	float x = clients[npc_id].x;

	if (clients[npc_id].move_stack == 10) {
		clients[npc_id].move_stack = 0;
		clients[npc_id].move_degree = -1;
	}

	if (clients[npc_id].move_degree == -1) {
		clients[npc_id].move_degree = uid(dre);
	}

	if (clients[npc_id].move_stack != 10) {
		switch (clients[npc_id].move_degree)
		{
		case 0:
			x++;
			clients[npc_id].degree = 270;
			break;
		case 1:
			x--;
			clients[npc_id].degree = 90;
			break;
		case 2:
			z++;
			clients[npc_id].degree = 180;
			break;
		case 3:
			z--;
			clients[npc_id].degree = 0;
			break;
		default:
			break;
		}
		clients[npc_id].move_stack++;
	}

	clients[npc_id].z = z;
	clients[npc_id].x = x;
}