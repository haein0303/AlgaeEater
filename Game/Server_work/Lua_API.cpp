
#include "stdafx.h"
#include "Lua_API.h"
#include "Session.h"
#include "Timer.h"

extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern lua_State* L;


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

int API_Cube(lua_State* L)
{
	int target_id = lua_tonumber(L, -1);
	lua_pop(L, 2);

	add_timer(0, 5000, EV_CB, target_id);
	return 0;
}

int API_Rush(lua_State* L)
{
	int client_id = lua_tonumber(L, -2);
	int npc_id = lua_tonumber(L, -1);
	lua_pop(L, 3);

	add_timer(npc_id, 10000, EV_RUSH, client_id);
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

void reset_lua()
{
	lua_close(L);

	L = luaL_newstate();
	clients[MAX_USER + NPC_NUM - 1].L = L;

	luaL_openlibs(L);
	if (luaL_loadfile(L, "hello.lua")) {
		printf("소환 실패");
	}
	else {
		printf("소환 성공");
	}
	lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "set_object_id");
	lua_pushnumber(L, MAX_USER + NPC_NUM - 1);
	lua_pcall(L, 1, 0, 0);

	lua_register(L, "API_get_x", API_get_x);
	lua_register(L, "API_get_z", API_get_z);
	lua_register(L, "API_Rush", API_Rush);
	lua_register(L, "API_Cube", API_Cube);
	lua_register(L, "API_get_state", API_get_state);
}