#pragma once
#include <mutex>
#include <thread>
#include <iostream>
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
	// 과제용 임시 변수
	int move_stack;
	int move_degree;
	// 루아 사용 변수
	lua_State* L;

public:
	SESSION();
	~SESSION();
	void do_recv();
	void do_send(void* packet);
	void send_login_ok_packet(int c_id, float x, float y, float z, float degree);
	void send_move_packet(int c_id, float x, float y, float z, float degree);
	void send_add_object(int c_id, float x, float y, float z, float degree, char* name);
	void send_remove_object(int c_id);
	void send_cube_add(int c_id, float x, float y, float z, float degree);
};

struct CUBE {
public:
	float x, y, z;
	float degree;
};