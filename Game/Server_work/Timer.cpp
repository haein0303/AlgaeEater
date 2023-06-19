#pragma once

#include "stdafx.h"
#include "Timer.h"

#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>

#include "Session.h"
#include "util.h"

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
			case EV_STAGE1_FIRST_BOSS:
			{
				cout << "기믹 판정 시작" << endl;
				int cnt_ck = 0;
				for (int i = 0; i < 4; i++) {
					if (clients[ev.object_id].color_sequence[i] == clients[ev.object_id].crash_sequence[i]) {
						cnt_ck++;
					}
				}

				if (cnt_ck == 4) { // 보스 기믹 성공
					int pl_num = 0;
					if (clients[0]._Room_Num != 999)
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER;
					else
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER + 1;

					cout << "기믹 성공" << endl;

					/*for (int i = pl_num; i < pl_num + ROOM_USER; i++) {
						char msg[NAME_SIZE] = "기믹 성공";

						clients[i].send_msg(msg);
					}*/

					// 보스 그로기
					clients[ev.object_id].char_state = AN_DEAD;
					clients[ev.object_id].boss_shield_trigger = false;

					int pl = ev.object_id;

					for (int i = pl_num; i < pl_num + ROOM_USER; ++i) {
						clients[i].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
							clients[pl].hp, clients[pl].char_state, 0, 0);
					}
					
					add_timer(ev.object_id, 10000, EV_BOSS_CON, ev.target_id);
				}
				else { // 보스 기믹 실패
					int pl_num = 0;
					if (clients[0]._Room_Num != 999)
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER;
					else
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER + 1;

					cout << "기믹 실패" << endl;

					for (int i = pl_num; i < pl_num + ROOM_USER; i++) {
						clients[pl_num]._sl.lock();
						if (clients[pl_num]._s_state != ST_INGAME) {
							clients[pl_num]._sl.unlock();
							continue;
						}
						
						clients[pl_num].hp /= 2;
						Update_Player(pl_num);
						clients[pl_num]._sl.unlock();
					}

					add_timer(ev.object_id, 10000, EV_BOSS_CON, ev.target_id);

					/*for (int i = pl_num; i < pl_num + ROOM_USER; i++) {
						char msg[NAME_SIZE] = "기믹 실패";

						clients[i].send_msg(msg);
					}*/
				}
				break;
			}
			case EV_STAGE1_SECOND_BOSS:
			{
				// 타겟 랜덤으로 돌려야 함
				int tar_id = 0;

				if (clients[tar_id]._Room_Num == 999) tar_id = 1;

				/*if (clients[tar_id].char_state == AN_DEAD) {
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
				}*/

				lua_getglobal(clients[ev.object_id].L, "event_rush");
				lua_pushnumber(clients[ev.object_id].L, tar_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);
				break;
			}
			case EV_NPC_CON:
				clients[ev.object_id]._sl.lock();
				if (clients[ev.object_id]._s_state == ST_FREE) {
					clients[ev.object_id]._sl.unlock();
					break;
				}
				clients[ev.object_id]._sl.unlock();
				/*if (clients[ev.target_id].char_state == AN_DEAD) {
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
				}*/

				ex_over->_comp_type = OP_NPC_CON;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);

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

				/*clients[ev.target_id]._sl.lock();
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
				clients[ev.target_id]._sl.unlock();*/

				if (clients[ev.object_id].hp <= 150000 && clients[ev.object_id].first_pattern == false) { // 첫번째 전멸기
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state != ST_INGAME) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						/*char msg[NAME_SIZE] = "기둥 파괴";

						clients[pl].send_msg(msg);*/
					}

					add_timer(ev.object_id, 40000, EV_STAGE1_FIRST_BOSS, ev.target_id);
					add_timer(ev.object_id, 1000, EV_BOSS_EYE, ev.target_id);
					clients[ev.object_id].first_pattern = true;
					break;
				}

				if (clients[ev.object_id].hp <= 50000 && clients[ev.object_id].second_pattern == false) { // 두번째 전멸기
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state != ST_INGAME) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						char msg[NAME_SIZE] = "기둥 돌진";

						clients[pl].send_msg(msg);
					}

					send_second_cube(ev.object_id, clients[ev.object_id].x, clients[ev.object_id].y, clients[ev.object_id].z);
					add_timer(ev.object_id, 10000, EV_STAGE1_SECOND_BOSS, ev.target_id);
					clients[ev.object_id].second_pattern = true;
					break;
				}

				/*if (clients[ev.target_id].char_state == AN_DEAD) {
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
				}*/

				lua_getglobal(clients[ev.object_id].L, "wander_boss");
				lua_pushnumber(clients[ev.object_id].L, ev.target_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				add_timer(ev.object_id, 100, EV_BOSS_CON, ev.target_id);
				break;
			}
			case EV_BOSS_EYE: {
				int pl = ev.object_id;

				// 여기에서 보스가 중간으로 텔포 하도록 해줘야 함, 근데 보스방 센터 위치를 몰?루

				if (clients[pl].color_sequence[0] == 0) {

					clients[pl].boss_shield_trigger = true;

					send_cube(pl, clients[pl].x, clients[pl].y, clients[pl].z);

					srand((unsigned int)time(NULL));

					for (int i = 0; i < 3; i++) {
						clients[pl].color_sequence[i] = i + 1;
					}

					clients[pl].color_sequence[3] = 0;
				}

				clients[ev.target_id].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
					clients[pl].hp, clients[pl].char_state, clients[pl].color_sequence[clients[pl].eye_color], 0);

				cout << clients[pl].color_sequence[clients[pl].eye_color] << endl;

				clients[pl].eye_color++;
				if (clients[pl].eye_color > 3) {
					clients[pl].eye_color = 0;
					break;
				}

				add_timer(pl, 3000, EV_BOSS_EYE, ev.target_id);
				break;
			}
			default:
				break;
			}
		}
	}
}