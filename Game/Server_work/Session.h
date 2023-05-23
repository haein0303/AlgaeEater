#pragma once
#include <mutex>
#include <thread>
#include <iostream>
#include <unordered_set>
#include <array>
#include "protocol.h"
#include "Over_EXP.h"

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment (lib, "lua54.lib")

enum SESSION_STATE { ST_FREE, ST_ACCEPTED, ST_INGAME };

enum SESSION_TYPE { TY_PLAYER, TY_MOVE_NPC, TY_HOLD_NPC, TY_BOSS };

enum SESSION_ANIM_STATE { AN_IDLE, AN_WALK, AN_ATTACK, AN_DEAD };

class SESSION {
protected:
	OVER_EXP _recv_over;
public:
	SESSION_STATE _s_state;
	int _id;
	SOCKET _socket;
	float	x, y, z;
	float	degree;
	char	_name[NAME_SIZE];
	int		_prev_remain;
	std::mutex	_sl;
	int		hp;
	int		char_state;
	int		stage;
	unsigned int client_time;
	SESSION_TYPE _object_type;

	// 스테이지 1 변수
	int color;
	int eye_color;

	// 임시 변수
	float start_x, start_z;
	float target_x, target_z;
	int turn;
	int stack;
	// 루아 사용 변수
	lua_State* L;
	int		_Room_Num;
	std::unordered_set <int> room_list;
	std::unordered_set <int> object_list;
	bool	Lua_on;

public:
	SESSION();
	~SESSION();
	void do_recv();
	void do_send(void* packet);
	void send_login_ok_packet(int c_id, float x, float y, float z, float degree, int hp);
	void send_move_packet(int c_id, float x, float y, float z, float degree, int hp, int state, unsigned int client_time);
	void send_add_object(int c_id, float x, float y, float z, float degree, char* name, int hp, int state, int ob_type);
	void send_remove_object(int c_id);
	void send_cube_add(int c_id, float x, float y, float z, float degree);
	void send_boss_add(int c_id, float x, float y, float z, float degree, char* name, int hp, int state);
	void send_boss_move(int c_id, float x, float y, float z, float degree, int hp, int state, int eye, unsigned int client_time);
	void send_msg(char* msg);
	void send_key(int c_id, float x, float y, float z, int color);
};

struct CUBE {
public:
	float x, y, z;
	float degree;
	int color;
	int		_Room_Num;
};

struct KEY {
public:
	float x, y, z;
	int color;
	int		_Room_Num;
};