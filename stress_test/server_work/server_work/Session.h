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
	unsigned int client_time;

	// 임시 변수
	float start_x, start_z;
	// 루아 사용 변수
	lua_State* L;
	int		_Room_Num;
	std::unordered_set <int> room_list;
	bool	Lua_on;

public:
	SESSION();
	~SESSION();
	void do_recv();
	void do_send(void* packet);
	void send_login_ok_packet(int c_id, float x, float y, float z, float degree, int hp);
	void send_move_packet(int c_id, float x, float y, float z, float degree, int hp, int state, unsigned int client_time);
	void send_add_object(int c_id, float x, float y, float z, float degree, char* name, int hp, int state);
	void send_remove_object(int c_id);
	void send_cube_add(int c_id, float x, float y, float z, float degree);
};

struct CUBE {
public:
	float x, y, z;
	float degree;
	int		_Room_Num;
};