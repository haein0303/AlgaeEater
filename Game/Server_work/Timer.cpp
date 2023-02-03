#pragma once

#include "stdafx.h"
#include "Timer.h"

#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>

#include "Session.h"

using namespace std;
using namespace chrono;

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
				//int rd_id = rand() % MAX_USER;
				int rd_id = 0;

				if (clients[rd_id]._s_state == ST_INGAME) {
					lua_getglobal(clients[MAX_USER + NPC_NUM - 1].L, "event_rush");
					lua_pushnumber(clients[MAX_USER + NPC_NUM - 1].L, rd_id);
					lua_pcall(clients[MAX_USER + NPC_NUM - 1].L, 1, 0, 0);

					lua_getglobal(clients[MAX_USER + NPC_NUM - 1].L, "create_cube");
					lua_pushnumber(clients[MAX_USER + NPC_NUM - 1].L, MAX_USER + NPC_NUM - 1);
					lua_pcall(clients[MAX_USER + NPC_NUM - 1].L, 1, 0, 0);
				}
				else {
					add_timer(0, 10, EV_CK, 0);
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
			case EV_UP:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_UPDATE;
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