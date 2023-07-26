#pragma once
#include<iostream>
#include<chrono>
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

enum BOSS_NUM {
	TEST,
	STAGE1,
	STAGE2,
	STAGE3,
	COUNT
};
constexpr int BOSS_HP[4] = { 5000,5000,5000,5000 };

#define LOBBY_SERVER_IP "127.0.0.1"
#define GAME_SERVER_IP "127.0.0.1"
constexpr int GAME_SERVER_PORT_NUM = 4000;
constexpr int LOBBY_SERVER_PORT_NUM = 4001;

constexpr int ROOM_NUM = 100;
constexpr int ROOM_USER = 4;
constexpr int ROOM_NPC = 50;
constexpr int ROOM_CUBE = 4;
constexpr int ROOM_KEY = 4;
constexpr int ROOM_FIELD = 60;

constexpr int MAX_USER = ROOM_NUM * ROOM_USER;
constexpr int NPC_NUM = ROOM_NUM * ROOM_NPC;
constexpr int CUBE_NUM = ROOM_NUM * ROOM_CUBE;
constexpr int KEY_NUM = ROOM_NUM * ROOM_KEY;
constexpr int FIELD_NUM = ROOM_NUM * ROOM_FIELD;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CONSOLE = 2;
constexpr char CS_COLLISION = 3;
constexpr char CS_KEY = 4;
constexpr char CS_OBJECT_COLLISION = 5;
constexpr char CS_GOD_MOD = 9;

constexpr char SC_LOGIN_OK = 10;
constexpr char SC_ADD_OBJECT = 11;
constexpr char SC_REMOVE_OBJECT = 12;
constexpr char SC_MOVE_OBJECT = 13;
constexpr char SC_ADD_CUBE = 14;
constexpr char SC_ADD_BOSS = 15;
constexpr char SC_MOVE_BOSS = 16;
constexpr char SC_MSG = 17;
constexpr char SC_KEY = 18;
constexpr char SC_DOOR = 19;
constexpr char SC_BOSS_SHIELD = 20;
constexpr char SC_BOSS_RUSH_TARGET = 21;
constexpr char SC_BOSS_SKILL_START = 22;
constexpr char SC_BOSS_SKILL_END = 23;
constexpr char SC_BOSS_PLAYER_CON = 24;
///////////////////////////////////////////

constexpr char SS_CONNECT_SERVER = 28;
constexpr char SS_DATA_PASS = 29;

constexpr char LCS_LOGIN = 30;
constexpr char LCS_MATCH = 31;
constexpr char LCS_OUT = 32;
constexpr char LCS_JOIN = 33;

constexpr char LSC_LOGIN_OK = 40;
constexpr char LSC_LOGIN_FAIL = 41;
constexpr char LSC_CONGAME = 42;
constexpr char LSC_JOIN_OK = 43;
constexpr char LSC_JOIN_FAIL = 44;

constexpr float PI = 3.1415926535;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	int		stage;
	int		character_type;
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	float	x;
	float	y;
	float	z;
	float	degree;
	int		char_state;
	unsigned int client_time;
};

struct CS_CONSOLE_PACKET {
	unsigned char size;
	char	type;
	int console;
};

struct CS_COLLISION_PACKET {
	unsigned char size;
	char	type;
	int		attack_type;
	int		attacker_id;
	int		target_id;
};

struct CS_KEY_PACKET {
	unsigned char size;
	char	type;
	int		color;
	int		key_id;
};

struct CS_OBJECT_COLLISION_PACKET {
	unsigned char size;
	char	type;
	int		attacker_id;
	int		target_id;
	int		object_type; // 0�� ť�� 1�� ����
};

struct CS_GOD_MOD_PACKET {
	unsigned char size;
	char type;
};

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	int		hp;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		hp;
	int		char_state;
	int		object_type;
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		ob_type;
	int		id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	int		hp;
	int		char_state;
	unsigned int client_time;
};

struct SC_ADD_CUBE_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		color;
	int		hp;
};

struct SC_ADD_BOSS_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
	int		hp;
	int		char_state;
};

struct SC_MOVE_BOSS_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	int		hp;
	int		char_state;
	int		eye_color;
	unsigned int client_time;
};

struct SC_MSG_PACKET {
	unsigned char size;
	char	type;
	char	msg[NAME_SIZE];
};

struct SC_KEY_PACKET {
	unsigned char size;
	char	type;
	float	x;
	float	y;
	float	z;
	int		id;
	int		color;
};

struct SC_DOOR_PACKET {
	unsigned char size;
	char	type;
};

struct SC_BOSS_SHIELD_PACKET {
	unsigned char size;
	char	type;
	int		shield_hp;
	bool	trigger;
};

struct SC_BOSS_RUSH_TARGET_PACKET {
	unsigned char size;
	char	type;
	int		target_id;
	bool	trigger;
};

struct SC_BOSS_SKILL_START_PACKET {
	unsigned char size;
	char	type;
	int		fd_id;
	float	x;
	float	z;
	float	r;
	int		fd_type;
	int		char_state;
};

struct SC_BOSS_SKILL_END_PACKET {
	unsigned char size;
	char	type;
	int		fd_id;
};

struct SC_BOSS_PLAYER_CON_PACKET {
	unsigned char size;
	char	type;
	int		con_num;   // 0 �� ���ο� 1�� ������ 2�� ����
};

struct SS_CONNECT_SERVER_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct SS_DATA_PASS_PACKET {
	unsigned char size;
	char type;
	char	name[NAME_SIZE];
};

struct LCS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	id[NAME_SIZE];
	char	passward[NAME_SIZE];
};

struct LCS_MATCH_PACKET {
	unsigned char size;
	char	type;
	int		stage;
};

struct LCS_OUT_PACKET {
	unsigned char size;
	char	type;
};

struct LCS_JOIN_PACKET {
	unsigned char size;
	char	type;
	char	id[NAME_SIZE];
	char	passward[NAME_SIZE];
};

struct LSC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		level;
};

struct LSC_LOGIN_FAIL_PACKET {
	unsigned char size;
	char	type;
};

struct LSC_CONGAME_PACKET {
	unsigned char size;
	char	type;
	bool	connect;
};

struct LSC_JOIN_OK_PACKET {
	unsigned char size;
	char	type;
};

struct LSC_JOIN_FAIL_PACKET {
	unsigned char size;
	char	type;
};
#pragma pack (pop)