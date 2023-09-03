
#include "stdafx.h"
#include "Lua_API.h"
#include "Session.h"
#include "Timer.h"
#include "util.h"

extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;
extern HANDLE g_h_iocp;
extern array<BOOL, ROOM_NUM> RESET_ROOM_NUM;
extern array<BOOL, ROOM_NUM> RELOAD_LUA;

extern class stage_npc_death_counts {
public:
	int counts;
};

extern array<stage_npc_death_counts, ROOM_NUM> death_counts;

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

int API_PLAYER_SET(lua_State* L)
{
	int player_id = lua_tonumber(L, -5);
	int player_max_hp = lua_tonumber(L, -4);
	int player_hp = lua_tonumber(L, -3);
	int player_atk = lua_tonumber(L, -2);
	int player_skill = lua_tonumber(L, -1);
	lua_pop(L, 6);

	clients[player_id].hp = player_hp;
	clients[player_id].max_hp = player_max_hp;
	clients[player_id].atk = player_atk;
	clients[player_id].skill_atk = player_skill;

	cout << "플레이어 아이디 : " << player_id << "    플레이어 atk : " << player_atk << endl;

	return 0;
}

int API_BOSS_SET(lua_State* L)
{
	int boss_id = lua_tonumber(L, -8);
	int boss_max_hp = lua_tonumber(L, -7);
	int boss_hp = lua_tonumber(L, -6);
	int boss_atk = lua_tonumber(L, -5);

	bool boss_first_pattern = lua_toboolean(L, -4);
	bool boss_second_pattern = lua_toboolean(L, -3);
	bool boss_third_pattern = lua_toboolean(L, -2);
	bool boss_fourth_pattern = lua_toboolean(L, -1);

	lua_pop(L, 9);

	clients[boss_id].hp = boss_hp;
	clients[boss_id].max_hp = boss_max_hp;
	clients[boss_id].atk = boss_atk;
	clients[boss_id].first_pattern = boss_first_pattern;
	clients[boss_id].second_pattern = boss_second_pattern;
	clients[boss_id].third_pattern = boss_third_pattern;
	clients[boss_id].fourth_pattern = boss_fourth_pattern;

	cout << "보스 아이디 : " << boss_id << "    보스 atk : " << boss_atk << endl;

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

void reset_player(int pl_id)
{
	lua_close(clients[pl_id].L);

	clients[pl_id].L = luaL_newstate();

	luaL_openlibs(clients[pl_id].L);
	luaL_loadfile(clients[pl_id].L, "hello.lua");
	lua_pcall(clients[pl_id].L, 0, 0, 0);

	lua_getglobal(clients[pl_id].L, "set_object_id");
	lua_pushnumber(clients[pl_id].L, pl_id);
	lua_pcall(clients[pl_id].L, 1, 0, 0);

	lua_register(clients[pl_id].L, "API_PLAYER_SET", API_PLAYER_SET);

	lua_getglobal(clients[pl_id].L, "set_player");
	lua_pushnumber(clients[pl_id].L, clients[pl_id]._object_type);
	lua_pcall(clients[pl_id].L, 1, 0, 0);

	Update_Player(pl_id);
}

void reset_boss_pattern(int c_id) 
{
	int npc_id = clients[c_id]._Room_Num * ROOM_NPC + MAX_USER + ROOM_NPC - 1;
	lua_close(clients[npc_id].L);

	clients[npc_id].L = luaL_newstate();

	luaL_openlibs(clients[npc_id].L);
	luaL_loadfile(clients[npc_id].L, "hello.lua");
	lua_pcall(clients[npc_id].L, 0, 0, 0);

	lua_getglobal(clients[npc_id].L, "set_object_id");
	lua_pushnumber(clients[npc_id].L, npc_id);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_BOSS_SET", API_BOSS_SET);

	lua_getglobal(clients[npc_id].L, "set_boss");
	lua_pushnumber(clients[npc_id].L, clients[npc_id]._object_type);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_get_x", API_get_x);
	lua_register(clients[npc_id].L, "API_get_z", API_get_z);
	lua_register(clients[npc_id].L, "API_Rush", API_Rush);
	lua_register(clients[npc_id].L, "API_get_state", API_get_state);
	lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
	lua_register(clients[npc_id].L, "API_Wander", API_Wander);

	RELOAD_LUA[clients[c_id]._Room_Num] = false;

	add_timer(npc_id, 1000, EV_BOSS_CON, c_id, clients[c_id]._Room_Num);
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
		printf("루아 스크립트 로드 실패 \n");
	}
	else {
		printf("루아 스크립트 로드 성공 \n");
	}

	lua_pcall(clients[npc_id].L, 0, 0, 0);

	lua_getglobal(clients[npc_id].L, "set_object_id");
	lua_pushnumber(clients[npc_id].L, npc_id);
	lua_pcall(clients[npc_id].L, 1, 0, 0);

	lua_register(clients[npc_id].L, "API_get_x", API_get_x);
	lua_register(clients[npc_id].L, "API_get_z", API_get_z);
	lua_register(clients[npc_id].L, "API_Rush", API_Rush);
	lua_register(clients[npc_id].L, "API_get_state", API_get_state);
	lua_register(clients[npc_id].L, "API_Tracking", API_Tracking);
	lua_register(clients[npc_id].L, "API_Wander", API_Wander);

	RELOAD_LUA[clients[c_id]._Room_Num] = false;

	add_timer(npc_id, 1000, EV_BOSS_CON, c_id, clients[c_id]._Room_Num);
}

void close_lua(int c_id)
{
	cout << "방 초기화" << endl;

	int npc_start_num = clients[c_id]._Room_Num * ROOM_NPC + MAX_USER;
	int cube_num = clients[c_id]._Room_Num * ROOM_CUBE;


	for (int i = 0; i < ROOM_NUM; i++) {
		death_counts[i].counts = 0;
	}

	set_room();

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
		clients[i].third_pattern = false;
		clients[i].fourth_pattern = false;
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