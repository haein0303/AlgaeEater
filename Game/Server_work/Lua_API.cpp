
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

void close_lua(int npc_id)
{
	cout << "아무도 없음" << endl;

	lua_close(clients[npc_id].L);
	clients[npc_id]._s_state = ST_INGAME;
	clients[npc_id]._Room_Num = (npc_id - MAX_USER) / ROOM_NPC;
	clients[npc_id].y = 0;
	clients[npc_id].degree = 0;
	clients[npc_id].start_x = 0;
	clients[npc_id].start_z = 0;
	clients[npc_id].hp = 100;
	clients[npc_id].char_state = AN_IDLE;
	clients[npc_id]._name[0] = 0;
	clients[npc_id]._prev_remain = 0;
	clients[npc_id].Lua_on = false;
	clients[npc_id].stage = 0;
	clients[npc_id].turn = 0;
	clients[npc_id].room_list.clear();
	clients[npc_id].object_list.clear();

	clients[npc_id].L = luaL_newstate();

	luaL_openlibs(clients[npc_id].L);

	luaL_loadfile(clients[npc_id].L, "hello.lua");
	lua_pcall(clients[npc_id].L, 0, 0, 0);

	lua_getglobal(clients[npc_id].L, "set_object_id");
	lua_pushnumber(clients[npc_id].L, npc_id);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_get_x", API_get_x);
	lua_register(clients[npc_id].L, "API_get_z", API_get_z);
	lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
	lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
	lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);

	int st = (npc_id - MAX_USER - clients[npc_id]._Room_Num * ROOM_NPC);

	if (st > 9) clients[npc_id]._object_type = TY_HOLD_NPC;
	else clients[npc_id]._object_type = TY_MOVE_NPC;

	switch (st)
	{
	case 10:
	case 0:
		clients[npc_id].x = -20;
		clients[npc_id].z = -20;
		break;
	case 11:
	case 1:
		clients[npc_id].x = -20;
		clients[npc_id].z = 0;
		break;
	case 12:
	case 2:
		clients[npc_id].x = -20;
		clients[npc_id].z = 20;
		break;
	case 13:
	case 3:
		clients[npc_id].x = 0;
		clients[npc_id].z = -20;
		break;
	case 14:
	case 4:
		clients[npc_id].x = 0;
		clients[npc_id].z = 0;
		break;
	case 15:
	case 5:
		clients[npc_id].x = 0;
		clients[npc_id].z = 20;
		break;
	case 16:
	case 6:
		clients[npc_id].x = 20;
		clients[npc_id].z = -20;
		break;
	case 17:
	case 7:
		clients[npc_id].x = 20;
		clients[npc_id].z = 0;
		break;
	case 18:
	case 8:
		clients[npc_id].x = 20;
		clients[npc_id].z = 20;
		break;
	case 19: // 얘가 돌진하는 애
	{
		clients[npc_id].x = 30;
		clients[npc_id].z = 30;
		clients[npc_id].eye_color = 0;
		clients[npc_id]._object_type = TY_BOSS_1;

		lua_register(clients[npc_id].L, "API_Wander", API_Wander);
		lua_register(clients[npc_id].L, "API_Rush", API_Rush);
		lua_register(clients[npc_id].L, "API_get_state", API_get_state);
		break;
	}
	default:
		break;
	}

	clients[npc_id].start_x = clients[npc_id].x;
	clients[npc_id].start_z = clients[npc_id].z;

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
}