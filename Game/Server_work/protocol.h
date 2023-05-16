#pragma once
#include<iostream>
#include<chrono>
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

#define LOBBY_SERVER_IP "127.0.0.1"
#define GAME_SERVER_IP "127.0.0.1"
constexpr int GAME_SERVER_PORT_NUM = 4000;
constexpr int LOBBY_SERVER_PORT_NUM = 4001;

constexpr int ROOM_NUM = 100;
constexpr int MAX_USER = ROOM_NUM * 4;
constexpr int NPC_NUM = ROOM_NUM * 10;
constexpr int CUBE_NUM = ROOM_NUM * 5;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CONSOLE = 2;
constexpr char CS_COLLISION = 3;
constexpr char CS_COLOR = 4;

constexpr char SC_LOGIN_OK = 11;
constexpr char SC_ADD_OBJECT = 12;
constexpr char SC_REMOVE_OBJECT = 13;
constexpr char SC_MOVE_OBJECT = 14;
constexpr char SC_ADD_CUBE = 15;
constexpr char SC_ADD_BOSS = 16;
constexpr char SC_MOVE_BOSS = 17;

constexpr char SS_CONNECT_SERVER = 20;
constexpr char SS_DATA_PASS = 21;

constexpr char LCS_LOGIN = 30;
constexpr char LCS_MATCH = 31;
constexpr char LCS_OUT = 32;

constexpr char LSC_LOGIN_OK = 40;
constexpr char LSC_CONGAME = 41;

constexpr float PI = 3.1415926535;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	int		stage;
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
	char	attack_type;
	int		attacker_id;
	int		target_id;
};

struct CS_COLOR_PACKET {
	unsigned char size;
	char	type;
	int		color;
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
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
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
	unsigned int client_time;
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

struct LSC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int		id;
};

struct LSC_CONGAME_PACKET {
	unsigned char size;
	char	type;
	bool	connect;
};
#pragma pack (pop)