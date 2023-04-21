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
		clients[i]._Room_Num = (i - MAX_USER) / 10;
		clients[i].y = 0;
		clients[i].degree = 0;
		clients[i].start_x = 0;
		clients[i].start_z = 0;
		clients[i].hp = 100;
		clients[i].char_state = 0;
		clients[i]._name[0] = 0;
		clients[i]._prev_remain = 0;
		clients[i].Lua_on = false;

		clients[i].L = luaL_newstate();

		luaL_openlibs(clients[i].L);

		luaL_loadfile(clients[i].L, "hello.lua");
		lua_pcall(clients[i].L, 0, 0, 0);

		lua_getglobal(clients[i].L, "set_object_id");
		lua_pushnumber(clients[i].L, i);
		lua_pcall(clients[i].L, 1, 0, 0);

		lua_register(clients[i].L, "API_get_x", API_get_x);
		lua_register(clients[i].L, "API_get_z", API_get_z);

		int st = (i - MAX_USER - clients[i]._Room_Num * 10);

		switch (st)
		{
		case 0:
			clients[i].x = -20;
			clients[i].z = -20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 1:
			clients[i].x = -20;
			clients[i].z = 0;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 2:
			clients[i].x = -20;
			clients[i].z = 20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 3:
			clients[i].x = 0;
			clients[i].z = -20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 4:
			clients[i].x = 0;
			clients[i].z = 0;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 5:
			clients[i].x = 0;
			clients[i].z = 20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 6:
			clients[i].x = 20;
			clients[i].z = -20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 7:
			clients[i].x = 20;
			clients[i].z = 0;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 8:
			clients[i].x = 20;
			clients[i].z = 20;

			lua_register(clients[i].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[i].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[i].L, "API_Tracking", API_Tracking);
			lua_register(clients[i].L, "API_Return", API_Return);
			break;
		case 9: // 얘가 돌진하는 애
		{
			clients[i].x = 30;
			clients[i].z = 30;

			lua_register(clients[i].L, "API_Rush", API_Rush);
			lua_register(clients[i].L, "API_Cube", API_Cube);
			lua_register(clients[i].L, "API_get_state", API_get_state);
			break;
		}
		default:
			break;
		}

		clients[i].start_x = clients[i].x;
		clients[i].start_z = clients[i].z;
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

void move_npc(int player_id, int c_id)
{
	// cout << player_id << ", " << c_id << endl;

	float x = clients[c_id].x;
	float z = clients[c_id].z;
	clients[c_id].char_state = 0;	
	// 이거 1로하면 애러남

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

	if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 2) {
		// 공격 처리 관련, 여기서 안 할 수도 있음
		//clients[c_id].char_state = 2;
	}
}

void return_npc(int c_id)
{
	float x = clients[c_id].x;
	float z = clients[c_id].z;
	clients[c_id].char_state = 0;

	float de = atan2(x - clients[c_id].start_x, z - clients[c_id].start_z);
	de = de * 180 / PI;
	clients[c_id].degree = de;

	if (x > clients[c_id].start_x) x--;
	else if (x < clients[c_id].start_x) x++;

	if (z > clients[c_id].start_z) z--;
	else if (z < clients[c_id].start_z) z++;

	if (abs(x - clients[c_id].start_x) < 1) x = clients[c_id].start_x;
	if (abs(z - clients[c_id].start_z) < 1) z = clients[c_id].start_z;

	if (x == clients[c_id].start_x && z == clients[c_id].start_z) clients[c_id].char_state = 0;

	clients[c_id].x = x;
	clients[c_id].z = z;
}
