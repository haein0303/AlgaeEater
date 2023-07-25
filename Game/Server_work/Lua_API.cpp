
#include "stdafx.h"
#include "Lua_API.h"
#include "Session.h"
#include "Timer.h"

extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;

extern HANDLE g_h_iocp;

int API_get_x(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float x = clients[obj_id].x;

	lua_pushnumber(L, x);
	return 1;
}

int API_get_z(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float z = clients[obj_id].z;

	lua_pushnumber(L, z);
	return 1;
}

int API_get_npc_x(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float x = clients[obj_id].start_x;

	lua_pushnumber(L, x);
	return 1;
}

int API_get_npc_z(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	float z = clients[obj_id].start_z;

	lua_pushnumber(L, z);
	return 1;
}

int API_Rush(lua_State* L)
{
	int npc_id = lua_tonumber(L, -3);
	float t_x = lua_tonumber(L, -2);
	float t_z = lua_tonumber(L, -1);
	lua_pop(L, 4);

	clients[npc_id].target_x = t_x;
	clients[npc_id].target_z = t_z;

	auto ex_over = new OVER_EXP;

	ex_over->_comp_type = OP_NPC_RUSH;
	ex_over->target_id = npc_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, npc_id, &ex_over->_over);

	return 0;
}

int API_get_state(lua_State* L)
{
	int obj_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	int state = clients[obj_id]._s_state;

	lua_pushnumber(L, state);
	return 1;
}

int API_Tracking(lua_State* L) 
{
	int player_id = lua_tonumber(L, -2);
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 3);

	auto ex_over = new OVER_EXP;

	ex_over->_comp_type = OP_NPC_MOVE;
	ex_over->target_id = player_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, npc_id, &ex_over->_over);
	return 0;
}

int API_Wander(lua_State* L)
{
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	auto ex_over = new OVER_EXP;
	ex_over->_comp_type = OP_BOSS_WANDER;
	ex_over->target_id = npc_id;
	PostQueuedCompletionStatus(g_h_iocp, 1, npc_id, &ex_over->_over);
	return 0;
}

void reset_lua(int c_id)
{
	int npc_id = clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC - 1;
	lua_close(clients[npc_id].L);

	clients[npc_id].x = clients[npc_id].start_x;
	clients[npc_id].z = clients[npc_id].start_z;

	clients[npc_id].L = luaL_newstate();

	luaL_openlibs(clients[npc_id].L);
	if (luaL_loadfile(clients[npc_id].L, "hello.lua")) {
		printf("소환 실패");
	}
	else {
		printf("소환 성공");
	}
	lua_pcall(clients[npc_id].L, 0, 0, 0);

	lua_getglobal(clients[npc_id].L, "set_object_id");
	lua_pushnumber(clients[npc_id].L, npc_id);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_get_x", API_get_x);
	lua_register(clients[npc_id].L, "API_get_z", API_get_z);
	lua_register(clients[npc_id].L, "API_Rush", API_Rush);
	lua_register(clients[npc_id].L, "API_get_state", API_get_state);
}

void close_lua(int c_id)
{
	cout << "방 초기화" << endl;

	int npc_start_num = clients[c_id]._Room_Num * ROOM_NPC + MAX_USER;
	int cube_num = clients[c_id]._Room_Num * ROOM_CUBE;

	for (int i = npc_start_num; i < npc_start_num + ROOM_NPC; i++) {
		if (i == npc_start_num + ROOM_NPC - 1)lua_close(clients[i].L);
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

		for (int j = cube_num; j < cube_num + CUBE_NUM; j++) {
			clients[i].object_list.insert(j);
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

	for (int i = npc_start_num; i < npc_start_num + ROOM_NPC; ++i) {
		for (int j = i + 1; j < i + ROOM_NPC; ++j) {
			if (j >= npc_start_num + ROOM_NPC) continue;
			else
			{
				clients[i].room_list.insert(j);
				clients[j].room_list.insert(i);
			};
		}
	}
	cout << "npc 재배치 끝" << endl;
}