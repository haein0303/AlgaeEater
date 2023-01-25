#pragma once

#include "NPC.h"
#include "stdafx.h"
#include "Session.h"
#include "Timer.h"
#include "Lua_API.h"


extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;
extern lua_State* L;
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, 4> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

void initialize_npc()
{
	for (int i = MAX_USER; i < MAX_USER + NPC_NUM - 1; ++i) {
		clients[i]._s_state = ST_INGAME;
		clients[i].x = (i - 10) * 1.5;
		clients[i].y = 0;
		clients[i].z = 0;
		clients[i].degree = 0;
		clients[i].move_stack = 0;
		clients[i].move_degree = 0;
		clients[i]._name[0] = 0;
		clients[i]._prev_remain = 0;
		add_timer(i, 5000, EV_MOVE, i);
	}

	clients[19]._s_state = ST_INGAME;
	clients[19].x = 9 * 1.5;
	clients[19].y = 0;
	clients[19].z = 0;
	clients[19].degree = 0;
	clients[19].move_stack = 0;
	clients[19].move_degree = 0;
	clients[19]._name[0] = 0;
	clients[19]._prev_remain = 0;

	L = luaL_newstate();
	clients[19].L = L;

	luaL_openlibs(L);
#if DEBUG
	if (luaL_loadfile(L, "hello.lua")) {
		printf("소환 실패");
	}
	else {
		printf("소환 성공");
	}
	#esle
		luaL_loadfile(L, "hello.lua")
#endif
		lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_object_id");
	lua_pushnumber(L, 19);
	lua_pcall(L, 1, 0, 0);

	lua_register(L, "API_get_x", API_get_x);
	lua_register(L, "API_get_z", API_get_z);
	lua_register(L, "API_Rush", API_Rush);
	lua_register(L, "API_Cube", API_Cube);
	lua_register(L, "API_get_state", API_get_state);

	add_timer(19, 10000, EV_CK, 0);
}

void initialize_cube(float x, float y, float z)
{
	for (int i = 0; i < 4; i++) {
		switch (i)
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

		for (int j = 0; j < MAX_USER; ++j) {
			auto& pl = clients[j];
			pl._sl.lock();
			if (ST_INGAME != pl._s_state) {
				pl._sl.unlock();
				continue;
			}
			pl.send_cube_add(i, cubes[i].x, cubes[i].y, cubes[i].z, cubes[i].degree);
			pl._sl.unlock();
		}
	}

	add_timer(19, 5000, EV_CB, 0);
}

void rush_npc(int player_id)
{
	float x = clients[19].x;
	float z = clients[19].z;

	Sleep(10);

	if (abs(x - clients[player_id].x) + abs(z - clients[player_id].z) <= 3) {
		add_timer(19, 10000, EV_RUSH, 0);
		return;
	}

	for (int i = 0; i < 4; i++) {
		if (abs(x - cubes[i].x) + abs(z - cubes[i].z) <= 3) {
			cout << "기둥 충돌" << endl;
			add_timer(19, 10000, EV_RUSH, 0);
			return;
		}
	}

	float de = atan2(x - clients[player_id].x, z - clients[player_id].z);
	de = de * 180 / PI;
	clients[19].degree = de;

	if (x > clients[player_id].x) x--;
	else if (x < clients[player_id].x) x++;

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

	for (int i = 0; i < MAX_USER; ++i) {
		auto& pl = clients;
		pl[i].send_move_packet(npc_id, clients[npc_id].x, clients[npc_id].y, clients[npc_id].z, clients[npc_id].degree);
	}
}