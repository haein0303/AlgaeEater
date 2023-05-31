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
				ex_over->_comp_type = OP_NPC_MOVE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			}
			case EV_RUSH:
			{
				ex_over->_comp_type = OP_NPC_RUSH;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			case EV_CK:
			{
				int tar_id = 0;

				if (clients[tar_id]._Room_Num == 999) tar_id = 1;

				if (clients[tar_id].char_state == AN_DEAD) {
					int dead_player = tar_id;
					for (auto& pl : clients[tar_id].room_list) {
						if (pl < MAX_USER) {
							if (clients[pl].char_state != AN_DEAD)
							{
								cout << "변경" << endl;
								tar_id = pl;
								break;
							}
						}
					}
					if (dead_player == tar_id) {
						clients[ev.object_id].char_state = AN_IDLE;
						ex_over->_comp_type = OP_SET_NPC;
						ex_over->target_id = tar_id;
						PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
						break;
					}
				}

				lua_getglobal(clients[ev.object_id].L, "event_rush");
				lua_pushnumber(clients[ev.object_id].L, tar_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				lua_getglobal(clients[ev.object_id].L, "create_cube");
				lua_pushnumber(clients[ev.object_id].L, ev.object_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);
				break;
			}
			case EV_CB:
			{
				ex_over->_comp_type = OP_CREATE_CUBE;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				break;
			}
			case EV_RETURN:
				ex_over->_comp_type = OP_NPC_RETURN;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			case EV_NPC_CON:
				cout << ev.target_id << endl;
				clients[ev.object_id]._sl.lock();
				if (clients[ev.object_id]._s_state == ST_FREE) {
					clients[ev.object_id]._sl.unlock();
					break;
				}
				clients[ev.object_id]._sl.unlock();
				if (clients[ev.target_id].char_state == AN_DEAD) {
					int dead_player = ev.target_id;
					for (auto& pl : clients[ev.target_id].room_list) {
						if (pl < MAX_USER) {
							if (clients[pl].char_state != AN_DEAD)
							{
								cout << "변경" << endl;
								ev.target_id = pl;
								break;
							}
						}
					}
					if (dead_player == ev.target_id) {
						clients[ev.object_id].char_state = AN_IDLE;
						ex_over->_comp_type = OP_SET_NPC;
						ex_over->target_id = ev.target_id;
						PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
						break;
					}
				}

				lua_getglobal(clients[ev.object_id].L, "tracking_player");
				lua_pushnumber(clients[ev.object_id].L, ev.target_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				add_timer(ev.object_id, 100, EV_NPC_CON, ev.target_id);
				break;
			case EV_BOSS_CON:
			{
				clients[ev.object_id]._sl.lock();
				if (clients[ev.object_id]._s_state == ST_FREE) {
					clients[ev.object_id]._sl.unlock();
					break;
				}
				clients[ev.object_id]._sl.unlock();

				clients[ev.target_id]._sl.lock();
				if (clients[ev.target_id]._s_state == ST_FREE) {
					clients[ev.target_id]._sl.unlock();
					int dead_player = ev.target_id;
					for (auto& pl : clients[ev.target_id].room_list) {
						if (pl < MAX_USER) {
							clients[pl]._sl.lock();
							if (clients[pl]._s_state == ST_INGAME)
							{
								clients[pl]._sl.unlock();
								cout << "변경" << endl;
								ev.target_id = pl;
								break;
							}
						}
					}
					if (dead_player == ev.target_id) {
						clients[ev.object_id].char_state = AN_DEAD;
						ex_over->_comp_type = OP_SET_NPC;
						ex_over->target_id = ev.target_id;
						PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
						break;
					}
				}
				clients[ev.target_id]._sl.unlock();

				if (clients[ev.object_id].hp <= 50 && clients[ev.object_id].first_pattern == false) {
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state != ST_INGAME) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						char msg[NAME_SIZE] = "돌진";

						clients[pl].send_msg(msg);
					}

					add_timer(ev.object_id, 5000, EV_CK, ev.target_id);
					add_timer(ev.object_id, 8000, EV_BOSS_CON, ev.target_id);
					clients[ev.object_id].first_pattern = true;
					break;
				}

				if (clients[ev.target_id].char_state == AN_DEAD) {
					int dead_player = ev.target_id;
					for (auto& pl : clients[ev.target_id].room_list) {
						if (pl < MAX_USER) {
							if (clients[pl].char_state != AN_DEAD)
							{
								cout << "변경" << endl;
								ev.target_id = pl;
								break;
							}
						}
					}
					if (dead_player == ev.target_id) {
						clients[ev.object_id].char_state = AN_DEAD;
						ex_over->_comp_type = OP_SET_NPC;
						ex_over->target_id = ev.target_id;
						PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
						break;
					}
				}

				lua_getglobal(clients[ev.object_id].L, "wander_boss");
				lua_pushnumber(clients[ev.object_id].L, ev.target_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				add_timer(ev.object_id, 100, EV_BOSS_CON, ev.target_id);
				break;
			}
			case EV_BOSS_EYE: {
				int pl = ev.object_id;
				clients[ev.target_id].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
					clients[pl].hp, clients[pl].char_state, clients[pl].eye_color, 0);

				clients[pl].eye_color++;
				if (clients[pl].eye_color > 4) clients[pl].eye_color = 0;

				add_timer(ev.object_id, 3000, EV_BOSS_EYE, ev.target_id);
				break;
			}
			case EV_WANDER: {
				ex_over->_comp_type = OP_BOSS_WANDER;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);
				break;
			}
			default:
				break;
			}
		}
	}
}