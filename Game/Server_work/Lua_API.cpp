
#include "stdafx.h"
#include "Lua_API.h"
#include "Session.h"
#include "Timer.h"

extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;

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

int API_Cube(lua_State* L)
{
	int target_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	add_timer(0, 100, EV_CB, target_id);
	return 0;
}

int API_Rush(lua_State* L)
{
	int npc_id = lua_tonumber(L, -3);
	float t_x = lua_tonumber(L, -2);
	float t_z = lua_tonumber(L, -1);
	lua_pop(L, 4);

	clients[npc_id].target_x = t_x;
	clients[npc_id].target_z = t_z;

	add_timer(npc_id, 100, EV_RUSH, npc_id);
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

	add_timer(npc_id, 10, EV_MOVE, player_id);
	return 0;
}

int API_Return(lua_State* L)
{
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	add_timer(npc_id, 10, EV_RETURN, npc_id);
	return 0;
}

int API_Wander(lua_State* L)
{
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	add_timer(npc_id, 10, EV_WANDER, npc_id);
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
	lua_register(clients[npc_id].L, "API_Cube", API_Cube);
	lua_register(clients[npc_id].L, "API_get_state", API_get_state);
}

void close_lua(int npc_id)
{
	cout << "아무도 없음" << endl;
	lua_close(clients[npc_id].L);

	for (int i = MAX_USER; i < MAX_USER + NPC_NUM; ++i) {
		clients[i]._s_state = ST_INGAME;
		clients[i]._Room_Num = (i - MAX_USER) / ROOM_NPC;
		clients[i].y = 0;
		clients[i].degree = 0;
		clients[i].start_x = 0;
		clients[i].start_z = 0;
		clients[i].hp = 100;
		clients[i].char_state = AN_IDLE;
		clients[i]._name[0] = 0;
		clients[i]._prev_remain = 0;
		clients[i].Lua_on = false;
		clients[i].stage = 0;
		clients[i].turn = 0;
		clients[i].room_list.clear();
		clients[i].object_list.clear();

		for (int j = 0; j < CUBE_NUM; j++) {
			if (cubes[j]._Room_Num != clients[i]._Room_Num) continue;
			else clients[i].object_list.insert(j);
		}

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
		lua_register(clients[i].L, "API_Return", API_Return);

		int st = (i - MAX_USER - clients[i]._Room_Num * ROOM_NPC);

		if (st > 9) clients[i]._object_type = TY_HOLD_NPC;
		else clients[i]._object_type = TY_MOVE_NPC;

		switch (st)
		{
		case 10:
		case 0:
			clients[i].x = -20;
			clients[i].z = -20;
			break;
		case 11:
		case 1:
			clients[i].x = -20;
			clients[i].z = 0;
			break;
		case 12:
		case 2:
			clients[i].x = -20;
			clients[i].z = 20;
			break;
		case 13:
		case 3:
			clients[i].x = 0;
			clients[i].z = -20;
			break;
		case 14:
		case 4:
			clients[i].x = 0;
			clients[i].z = 0;
			break;
		case 15:
		case 5:
			clients[i].x = 0;
			clients[i].z = 20;
			break;
		case 16:
		case 6:
			clients[i].x = 20;
			clients[i].z = -20;
			break;
		case 17:
		case 7:
			clients[i].x = 20;
			clients[i].z = 0;
			break;
		case 18:
		case 8:
			clients[i].x = 20;
			clients[i].z = 20;
			break;
		case 19: // 얘가 돌진하는 애
		{
			clients[i].x = 30;
			clients[i].z = 30;
			clients[i].eye_color = 0;
			clients[i]._object_type = TY_BOSS_1;

			lua_register(clients[i].L, "API_Wander", API_Wander);
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