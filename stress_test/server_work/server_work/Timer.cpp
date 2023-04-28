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
extern default_random_engine dre;
extern uniform_int_distribution<> uid;
extern array<SESSION, MAX_USER + NPC_NUM> clients;
extern array<CUBE, CUBE_NUM> cubes;
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

			auto ex_over = new OVER_EXP;

			switch (ev.ev)
			{
			case EV_MOVE:
			{
				//cout << "ev.obj : " << ev.object_id << ", ev.tar : " << ev.target_id << endl;
				ex_over->_comp_type = OP_NPC_MOVE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			}
			case EV_RUSH:
			{
				ex_over->_comp_type = OP_NPC_RUSH;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			}
			case EV_CK:
			{
				srand((unsigned int)time(NULL));
				int rd_id = clients[ev.object_id]._Room_Num * 4;

				if (clients[rd_id]._s_state == ST_INGAME) {
					lua_getglobal(clients[ev.object_id].L, "event_rush");
					lua_pushnumber(clients[ev.object_id].L, rd_id);
					lua_pcall(clients[ev.object_id].L, 1, 0, 0);

					lua_getglobal(clients[ev.object_id].L, "create_cube");
					lua_pushnumber(clients[ev.object_id].L, ev.object_id);
					lua_pcall(clients[ev.object_id].L, 1, 0, 0);
				}
				else {
					add_timer(0, 10, EV_CK, 0);
				}
				break;
			}
			case EV_CB:
			{
				ex_over->_comp_type = OP_CREATE_CUBE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			case EV_UP:
			{
				ex_over->_comp_type = OP_UPDATE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			case EV_RETURN:
				// cout << ev.target_id << ev.object_id << endl;
				ex_over->_comp_type = OP_NPC_RETURN;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			case EV_NPC_CON:
				//cout << ev.object_id << ", " << ev.target_id << endl;

				lua_getglobal(clients[ev.object_id].L, "tracking_player");
				lua_pushnumber(clients[ev.object_id].L, ev.target_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				add_timer(ev.object_id, 100, EV_NPC_CON, ev.target_id);
				break;
			default:
				break;
			}
		}
	}
}