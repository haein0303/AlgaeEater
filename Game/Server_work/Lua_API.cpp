
#include "stdafx.h"
#include "Lua_API.h"
#include "Session.h"
#include "Timer.h"

extern array<SESSION, MAX_USER + NPC_NUM> clients;


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
	lua_close(clients[npc_id].L);

	clients[npc_id]._s_state = ST_INGAME;
	clients[npc_id]._Room_Num = (npc_id - MAX_USER) / ROOM_NPC;
	clients[npc_id].y = 0;
	clients[npc_id].degree = 0;
	clients[npc_id].start_x = 0;
	clients[npc_id].start_z = 0;
	clients[npc_id].hp = 100;
	clients[npc_id].char_state = 0;
	clients[npc_id]._name[0] = 0;
	clients[npc_id]._prev_remain = 0;
	clients[npc_id].Lua_on = false;
	clients[npc_id].stage = 0;
	clients[npc_id].room_list.clear();

	clients[npc_id].L = luaL_newstate();

	luaL_openlibs(clients[npc_id].L);

	luaL_loadfile(clients[npc_id].L, "hello.lua");
	lua_pcall(clients[npc_id].L, 0, 0, 0);

	lua_getglobal(clients[npc_id].L, "set_object_id");
	lua_pushnumber(clients[npc_id].L, npc_id);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_get_x", API_get_x);
	lua_register(clients[npc_id].L, "API_get_z", API_get_z);

	int st = (npc_id - MAX_USER - clients[npc_id]._Room_Num * ROOM_NPC);

	switch (st)
		{
		case 0:
			clients[npc_id].x = -20;
			clients[npc_id].z = -20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 1:
			clients[npc_id].x = -20;
			clients[npc_id].z = 0;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 2:
			clients[npc_id].x = -20;
			clients[npc_id].z = 20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 3:
			clients[npc_id].x = 0;
			clients[npc_id].z = -20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 4:
			clients[npc_id].x = 0;
			clients[npc_id].z = 0;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 5:
			clients[npc_id].x = 0;
			clients[npc_id].z = 20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 6:
			clients[npc_id].x = 20;
			clients[npc_id].z = -20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 7:
			clients[npc_id].x = 20;
			clients[npc_id].z = 0;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 8:
			clients[npc_id].x = 20;
			clients[npc_id].z = 20;

			lua_register(clients[npc_id].L, "API_get_npc_x", API_get_npc_x);
			lua_register(clients[npc_id].L, "API_get_npc_z", API_get_npc_z);
			lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
			lua_register(clients[npc_id].L, "API_Return", API_Return);
			break;
		case 9: // 얘가 돌진하는 애
		{
			clients[npc_id].x = 30;
			clients[npc_id].z = 30;

			lua_register(clients[npc_id].L, "API_Rush", API_Rush);
			lua_register(clients[npc_id].L, "API_Cube", API_Cube);
			lua_register(clients[npc_id].L, "API_get_state", API_get_state);
			break;
		}
		default:
			break;
		}

	clients[npc_id].start_x = clients[npc_id].x;
	clients[npc_id].start_z = clients[npc_id].z;
}