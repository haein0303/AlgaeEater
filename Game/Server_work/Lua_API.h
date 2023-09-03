#pragma once

int API_get_x(lua_State* L);

int API_get_z(lua_State* L);

int API_get_npc_x(lua_State* L);

int API_get_npc_z(lua_State* L);

int API_Rush(lua_State* L);

int API_get_state(lua_State* L);

int API_Tracking(lua_State* L);

int API_Wander(lua_State* L);

int API_BOSS_SET(lua_State* L);

int API_PLAYER_SET(lua_State* L);

void reset_player(int pl_id);

void reset_boss_pattern(int c_id);

void reset_lua(int c_id);

void close_lua(int c_id);