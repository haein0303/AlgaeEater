#pragma once
constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

#define LOBBY_SERVER_IP "127.0.0.1"
constexpr int LOBBY_SERVER_PORT_NUM = 4001;

constexpr int ROOM_NUM = 100;
constexpr int MAX_USER = ROOM_NUM * 4;
constexpr int NPC_NUM = ROOM_NUM * 10;
constexpr int CUBE_NUM = ROOM_NUM * 4;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CONSOLE = 2;

constexpr char SC_LOGIN_OK = 11;
constexpr char SC_ADD_OBJECT = 12;
constexpr char SC_REMOVE_OBJECT = 13;
constexpr char SC_MOVE_OBJECT = 14;
constexpr char SC_ADD_CUBE = 15;

constexpr char SS_CONNECT_SERVER = 20;
constexpr char SS_DATA_PASS = 21;

constexpr float PI = 3.1415926535;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	float	x;
	float	y;
	float	z;
	float	degree;
	unsigned int client_time;
};

struct CS_CONSOLE_PACKET {
	unsigned char size;
	char	type;
	int console;
};

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int	id;
	float	x, y, z;
	float	degree;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
	char	name[NAME_SIZE];
};

struct SC_ADD_CUBE_PACKET {
	unsigned char size;
	char	type;
	int		id;
	float	x, y, z;
	float	degree;
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int	id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int	id;
	float	x, y, z;
	float	degree;
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
#pragma pack (pop)