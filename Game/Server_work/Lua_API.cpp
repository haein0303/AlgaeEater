
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

	add_timer(0, 5000, EV_CB, target_id);
	return 0;
}

int API_Rush(lua_State* L)
{
	int npc_id = lua_tonumber(L, -3);
	float t_x = lua_tonumber(L, -2);
	float t_z = lua_tonumber(L, -1);
	lua_pop(L, 4);

	clients[npc_id].start_x = t_x;
	clients[npc_id].start_z = t_z;

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

void reset_lua(int c_id)
{
	int npc_id = clients[c_id]._Room_Num * 10 + MAX_USER + 9;
	lua_close(clients[npc_id].L);

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