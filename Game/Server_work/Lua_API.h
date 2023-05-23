#pragma once

int API_get_x(lua_State* L);

int API_get_z(lua_State* L);

int API_get_npc_x(lua_State* L);

int API_get_npc_z(lua_State* L);

int API_Cube(lua_State* L);

int API_Rush(lua_State* L);

int API_get_state(lua_State* L);

int API_Return(lua_State* L);

int API_Tracking(lua_State* L);

int API_Wander(lua_State* L);

void reset_lua(int c_id);

void close_lua(int c_id);