#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include "Extern.h"

using namespace std;
using namespace chrono;

enum EVENT_TYPE { EV_MOVE, EV_RUSH, EV_CK, EV_CB };

struct TIMER_EVENT {
	int object_id;
	EVENT_TYPE ev;
	chrono::system_clock::time_point act_time;
	int target_id;

	constexpr bool operator < (const TIMER_EVENT& _Left) const
	{
		return (act_time > _Left.act_time);
	}

};

extern HANDLE g_h_iocp;
extern SOCKET g_s_socket;
extern lua_State* L;
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, 4> cubes;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id)
{
	TIMER_EVENT ev;
	ev.act_time = system_clock::now() + milliseconds(act_time);
	ev.object_id = obj_id;
	ev.ev = e_type;
	ev.target_id = target_id;

	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
}

void do_timer()
{
	while (true)
	{
		while (true)
		{
			timer_l.lock();

			if (timer_queue.empty() == true)
			{
				timer_l.unlock();
				break;
			}

			if (timer_queue.top().act_time > chrono::system_clock::now())
			{
				timer_l.unlock();
				break;
			}

			TIMER_EVENT ev = timer_queue.top();
			timer_queue.pop();
			timer_l.unlock();

			switch (ev.ev)
			{
			case EV_MOVE:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_NPC_MOVE;
				ex_over->target_id = ev.object_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				add_timer(ev.object_id, 100, ev.ev, ev.target_id);
				break;
			}
			case EV_RUSH:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_NPC_RUSH;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			case EV_CK:
			{
				srand((unsigned int)time(NULL));
				int rd_id = rand() % MAX_USER;

				if (clients[rd_id]._s_state == ST_INGAME) {
					lua_getglobal(clients[19].L, "event_rush");
					lua_pushnumber(clients[19].L, rd_id);
					lua_pcall(clients[19].L, 1, 0, 0);

					lua_getglobal(clients[19].L, "create_cube");
					lua_pushnumber(clients[19].L, 19);
					lua_pcall(clients[19].L, 1, 0, 0);
				}
				else {
					add_timer(19, 10, EV_CK, 0);
				}
				break;
			}
			case EV_CB:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_CREATE_CUBE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			default:
				break;
			}
		}
	}
}