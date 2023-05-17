#include "Session.h"

SESSION::SESSION()
{
	_id = -1;
	_socket = 0;
	x = 0;
	y = 0;
	z = 0;
	degree = 0;
	_name[0] = 0;
	char_state = 0;
	hp = 0;
	_s_state = ST_FREE;
	_prev_remain = 0;
	_Room_Num = 0;
	start_x = 0;
	start_z = 0;
	target_x = 0;
	target_z = 0;
	stage = 0;
	turn = 0;
	color = 0;
	eye_color = 0;
	Lua_on = false;
}

SESSION::~SESSION()
{

}

void SESSION::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
}

void SESSION::do_send(void* packet)
{
	OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void SESSION::send_login_ok_packet(int c_id, float x, float y, float z, float degree, int hp)
{
	SC_LOGIN_OK_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	p.x = x;
	p.y = y;
	p.z = z;
	p.hp = hp;
	do_send(&p);
}

void SESSION::send_move_packet(int c_id, float x, float y, float z, float degree, int hp, int state, unsigned int client_time)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.x = x;
	p.y = y;
	p.z = z;
	p.degree = degree;
	p.hp = hp;
	p.char_state = state;
	p.client_time = client_time;
	do_send(&p);
}

void SESSION::send_add_object(int c_id, float x, float y, float z, float degree, char* name, int hp, int state)
{
	SC_ADD_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ADD_OBJECT_PACKET);
	p.type = SC_ADD_OBJECT;
	p.x = x;
	p.y = y;
	p.z = z;
	p.degree = degree;
	strcpy_s(p.name, name);
	p.hp = hp;
	p.char_state = state;
	do_send(&p);
}

void SESSION::send_remove_object(int c_id)
{
	SC_REMOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_REMOVE_OBJECT_PACKET);
	p.type = SC_REMOVE_OBJECT;
	do_send(&p);
}

void SESSION::send_cube_add(int c_id, float x, float y, float z, float degree)
{
	SC_ADD_CUBE_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ADD_CUBE_PACKET);
	p.type = SC_ADD_CUBE;
	p.x = x;
	p.y = y;
	p.z = z;
	p.degree = degree;
	do_send(&p);
}

void SESSION::send_boss_add(int c_id, float x, float y, float z, float degree, char* name, int hp, int state)
{
	SC_ADD_BOSS_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_ADD_BOSS_PACKET);
	p.type = SC_ADD_BOSS;
	p.x = x;
	p.y = y;
	p.z = z;
	p.degree = degree;
	strcpy_s(p.name, name);
	p.hp = hp;
	p.char_state = state;
	do_send(&p);
}

void SESSION::send_boss_move(int c_id, float x, float y, float z, float degree, int hp, int state, int eye, unsigned int client_time)
{
	SC_MOVE_BOSS_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_BOSS_PACKET);
	p.type = SC_MOVE_BOSS;
	p.x = x;
	p.y = y;
	p.z = z;
	p.degree = degree;
	p.hp = hp;
	p.char_state = state;
	p.eye_color = eye;
	p.client_time = client_time;
	do_send(&p);
}

void SESSION::send_msg(char* msg)
{
	SC_MSG_PACKET p;
	p.size = sizeof(SC_MSG_PACKET);
	p.type = SC_MSG;
	strcpy_s(p.msg, msg);
	do_send(&p);
}
