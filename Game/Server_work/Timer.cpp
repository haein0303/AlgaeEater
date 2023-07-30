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
extern array<FIELD, FIELD_NUM> fields;
extern priority_queue<TIMER_EVENT> timer_queue;
extern mutex timer_l;
extern array<BOOL, ROOM_NUM> RESET_ROOM_NUM;
extern array<BOOL, ROOM_NUM> RELOAD_LUA;

int skill_tic = 0;

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id, int room_num)
{
	TIMER_EVENT ev;
	ev.act_time = system_clock::now() + milliseconds(act_time);
	ev.object_id = obj_id;
	ev.ev = e_type;
	ev.target_id = target_id;
	ev.room_num = room_num;

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

			if (true == RESET_ROOM_NUM[timer_queue.top().room_num])
			{
				timer_queue.pop();
				timer_l.unlock();
				break;
			}

			if (true == RELOAD_LUA[timer_queue.top().room_num])
			{
				timer_queue.pop();
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
					if (clients[0]._Room_Num != 9999)
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER;
					else
						pl_num = clients[ev.object_id]._Room_Num * ROOM_USER + 1;

					cout << "기믹 성공" << endl;

					/*for (int i = pl_num; i < pl_num + ROOM_USER; i++) {
						char msg[NAME_SIZE] = "기믹 성공";

						clients[i].send_msg(msg);
					}*/

					// 보스 그로기
					clients[ev.object_id].char_state = AN_DOWN;
					clients[ev.object_id].boss_shield_trigger = false;

					int pl = ev.object_id;

					for (int i = pl_num; i < pl_num + ROOM_USER; ++i) {
						clients[i].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
							clients[pl].hp, clients[pl].char_state, 0, 0);
					}
					
					add_timer(ev.object_id, 10000, EV_BOSS_CON, ev.target_id, clients[ev.object_id]._Room_Num);
					break;
				}
				else { // 보스 기믹 실패
					int pl_num = 0;
					if (clients[0]._Room_Num != 9999)
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
						clients[pl_num]._sl.unlock();
						
						clients[pl_num].hp /= 2;
						Update_Player(pl_num);
					}

					clients[ev.object_id].boss_shield_trigger = false;
					add_timer(ev.object_id, 1000, EV_BOSS_CON, ev.target_id, clients[ev.object_id]._Room_Num);

					break;

					/*for (int i = pl_num; i < pl_num + ROOM_USER; i++) {
						char msg[NAME_SIZE] = "기믹 실패";

						clients[i].send_msg(msg);
					}*/
				}
				break;
			}
			case EV_STAGE1_SECOND_BOSS:
			{
				int tar_id = ev.target_id;

				lua_getglobal(clients[ev.object_id].L, "event_rush");
				lua_pushnumber(clients[ev.object_id].L, tar_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);
				break;
			}
			case EV_STAGE2_FIRST_BOSS: {
				for (auto& pl : clients[ev.object_id].room_list) {
					if (pl >= MAX_USER) continue;
					clients[pl]._sl.lock();
					if (clients[pl]._s_state != ST_INGAME) {
						clients[pl]._sl.unlock();
						continue;
					}
					clients[pl]._sl.unlock();

					SC_BOSS_SKILL_END_PACKET p;
					p.size = sizeof(SC_BOSS_SKILL_END_PACKET);
					p.type = SC_BOSS_SKILL_END;
					p.fd_id = ev.target_id;

					clients[pl].do_send(&p);

					add_timer(ev.object_id, 10000, EV_BOSS_FIELD_OFF, ev.target_id, clients[ev.object_id]._Room_Num);
				}
				break;
			}
			case EV_STAGE3_BOSS: {
				SC_BOSS_PLAYER_CON_PACKET p;
				p.type = SC_BOSS_PLAYER_CON;
				p.size = sizeof(SC_BOSS_PLAYER_CON_PACKET);
				p.con_num = ev.target_id;
				p.trigger = true;
				p.x = 0;
				p.y = 0;
				p.z = 0;
				switch (ev.target_id)
				{
				case 0: { // 이속 감소
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					add_timer(ev.object_id, 30000, EV_BOSS_PLAYER_CON_OFF, ev.target_id, clients[ev.object_id]._Room_Num);
					break;
				}
				case 1: { // 조작 반전
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					add_timer(ev.object_id, 30000, EV_BOSS_PLAYER_CON_OFF, ev.target_id, clients[ev.object_id]._Room_Num);
					break;
				}
				case 2: { // 마비
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					add_timer(ev.object_id, 30000, EV_BOSS_PLAYER_CON_OFF, ev.target_id, clients[ev.object_id]._Room_Num);
					break;
				}
				case 3: { // 텔포
					int room_cnt = 0;
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();
						// 텔포방 좌표 넣어줘야 함
						switch (room_cnt)
						{
						case 0:
							p.x = 0;
							p.y = 0;
							p.z = 0;
							room_cnt++;
							break;
						case 1:
							p.x = 100;
							p.y = 0;
							p.z = 0;
							room_cnt++;
							break;
						case 2:
							p.x = 200;
							p.y = 0;
							p.z = 0;
							room_cnt++;
							break;
						case 3:
							p.x = 300;
							p.y = 0;
							p.z = 0;
							room_cnt++;
							break;
						default:
							break;
						}
						
						clients[pl].do_send(&p);
					}
					add_timer(ev.object_id, 10000, EV_BOSS_PLAYER_CON_OFF, ev.target_id, clients[ev.object_id]._Room_Num);
					break;
				}
				case 4: { // 회복
					clients[ev.object_id].hp = BOSS_HP[2] * 0.75;
					clients[ev.object_id].third_pattern = true;
					break;
				}
				default:
					break;
				}
				break;
			}
			case EV_NPC_CON:
				clients[ev.object_id]._sl.lock();
				if (clients[ev.object_id]._s_state == ST_FREE) {
					clients[ev.object_id]._sl.unlock();
					break;
				}
				clients[ev.object_id]._sl.unlock();

				if (clients[ev.object_id]._object_type != TY_BOSS_SKILL) {
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;
						if (pl == ev.target_id) continue;
						if (clients[pl].char_state == AN_DEAD) continue;

						float tar_abs = abs(clients[ev.object_id].x - clients[ev.target_id].x) + abs(clients[ev.object_id].z - clients[ev.target_id].z);
						float new_abs = abs(clients[ev.object_id].x - clients[pl].x) + abs(clients[ev.object_id].z - clients[pl].z);

						if (tar_abs > new_abs) ev.target_id = pl;
					}
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
						cout << "타겟 지정 불가" << endl;
						break;
					}
				}

				ex_over->_comp_type = OP_NPC_CON;
				ex_over->target_id = ev.target_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.object_id, &ex_over->_over);

				add_timer(ev.object_id, 100, EV_NPC_CON, ev.target_id, clients[ev.object_id]._Room_Num);

				break;
			case EV_BOSS_CON:
			{
				clients[ev.object_id]._sl.lock();
				if (clients[ev.object_id]._s_state == ST_FREE) {
					clients[ev.object_id]._sl.unlock();
					break;
				}
				clients[ev.object_id]._sl.unlock();

				for (auto& pl : clients[ev.object_id].room_list) {
					if (pl >= MAX_USER) continue;
					if (pl == ev.target_id) continue;
					if (clients[pl].char_state == AN_DEAD) continue;

					float tar_abs = abs(clients[ev.object_id].x - clients[ev.target_id].x) + abs(clients[ev.object_id].z - clients[ev.target_id].z);
					float new_abs = abs(clients[ev.object_id].x - clients[pl].x) + abs(clients[ev.object_id].z - clients[pl].z);

					if (tar_abs > new_abs) ev.target_id = pl;
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
						cout << "타겟 지정 불가" << endl;
						break;
					}
				}

				if (clients[ev.object_id].hp <= BOSS_HP[0] * 0.75 && clients[ev.object_id].first_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_1) { // 첫번째 전멸기
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

					add_timer(ev.object_id, 40000, EV_STAGE1_FIRST_BOSS, ev.target_id, clients[ev.object_id]._Room_Num);
					add_timer(ev.object_id, 1000, EV_BOSS_EYE, ev.target_id, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].first_pattern = true;
					break;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[0] * 0.25 && clients[ev.object_id].second_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_1) { 
					// 두번째 전멸기

					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state != ST_INGAME || clients[pl].char_state == AN_DEAD) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						ev.target_id = pl;
						break;
					}

					SC_BOSS_RUSH_TARGET_PACKET p;
					p.size = sizeof(SC_BOSS_RUSH_TARGET_PACKET);
					p.type = SC_BOSS_RUSH_TARGET;
					p.target_id = ev.target_id;
					p.trigger = true;

					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;

						clients[pl].do_send(&p);
					}

					send_second_cube(ev.object_id, clients[ev.object_id].x, clients[ev.object_id].y, clients[ev.object_id].z);
					add_timer(ev.object_id, 10000, EV_STAGE1_SECOND_BOSS, ev.target_id, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].second_pattern = true;
					break;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[1] * 0.75 && clients[ev.object_id].first_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_2) {

					std::random_device rd;
					std::uniform_int_distribution<int> dis(-20, 20);

					for (int i = clients[ev.object_id]._Room_Num * ROOM_FIELD; i < clients[ev.object_id]._Room_Num * ROOM_FIELD + ROOM_FIELD; i++) {
						fields[i].type = FD_SM_DMG;
						fields[i].r = 3;
						fields[i].x = clients[ev.object_id].x + dis(rd);
						fields[i].z = clients[ev.object_id].z + dis(rd);
					}

					add_timer(ev.object_id, 100, EV_BOSS_FIELD_ON, 0, clients[ev.object_id]._Room_Num);
					add_timer(ev.object_id, 10000, EV_BOSS_FIELD_ON, 1, clients[ev.object_id]._Room_Num);
					add_timer(ev.object_id, 20000, EV_BOSS_FIELD_ON, 2, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].first_pattern = true;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[1] * 0.25 && clients[ev.object_id].second_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_2) {
					for (int i = clients[ev.object_id]._Room_Num * ROOM_FIELD; i < clients[ev.object_id]._Room_Num * ROOM_FIELD + ROOM_FIELD; i++) {
						if (i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 10) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x - 25 + (i % ROOM_FIELD * 5);
							fields[i].z = clients[ev.object_id].z;
						}
						else if (i >= clients[ev.object_id]._Room_Num * ROOM_FIELD + 10 && i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 20) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x;
							fields[i].z = clients[ev.object_id].z - 25 + ((i % ROOM_FIELD - 10) * 5);
						}
						else if (i >= clients[ev.object_id]._Room_Num * ROOM_FIELD + 20 && i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 30) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x - 25 + ((i % ROOM_FIELD - 20) * 5);
							fields[i].z = clients[ev.object_id].z - 25 + ((i % ROOM_FIELD - 20) * 5);
						}
						else if (i >= clients[ev.object_id]._Room_Num * ROOM_FIELD + 30 && i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 40) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x - 25 + ((i % ROOM_FIELD - 30) * 5);
							fields[i].z = clients[ev.object_id].z + 25 - ((i % ROOM_FIELD - 30) * 5);
						}
						else if (i >= clients[ev.object_id]._Room_Num * ROOM_FIELD + 40 && i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 50) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x - 25 + ((i % ROOM_FIELD - 40) * 5);
							fields[i].z = clients[ev.object_id].z;
						}
						else if (i >= clients[ev.object_id]._Room_Num * ROOM_FIELD + 50 && i < clients[ev.object_id]._Room_Num * ROOM_FIELD + 60) {
							fields[i].type = FD_SM_DMG;
							fields[i].r = 3;
							fields[i].x = clients[ev.object_id].x;
							fields[i].z = clients[ev.object_id].z - 25 + ((i % ROOM_FIELD - 50) * 5);
						}
					}

					add_timer(ev.object_id, 100, EV_BOSS_FIELD_ON, 0, clients[ev.object_id]._Room_Num);
					add_timer(ev.object_id, 10000, EV_BOSS_FIELD_ON, 1, clients[ev.object_id]._Room_Num);
					add_timer(ev.object_id, 20000, EV_BOSS_FIELD_ON, 2, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].second_pattern = true;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[2] * 0.75 && clients[ev.object_id].first_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_3) {
					add_timer(ev.object_id, 100, EV_STAGE3_BOSS, 0, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].first_pattern = true;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[2] * 0.5 && clients[ev.object_id].second_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_3) {
					add_timer(ev.object_id, 100, EV_STAGE3_BOSS, 1, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].second_pattern = true;
				}

				if (clients[ev.object_id].hp <= BOSS_HP[2] * 0.25 && clients[ev.object_id].third_pattern == false && clients[ev.object_id]._object_type == TY_BOSS_3) {
					add_timer(ev.object_id, 100, EV_STAGE3_BOSS, 4, clients[ev.object_id]._Room_Num);
				}

				if (clients[ev.object_id].hp <= BOSS_HP[2] * 0.25 && clients[ev.object_id].fourth_pattern == false && clients[ev.object_id].third_pattern == true && clients[ev.object_id]._object_type == TY_BOSS_3) {
					add_timer(ev.object_id, 100, EV_STAGE3_BOSS, 3, clients[ev.object_id]._Room_Num);
					clients[ev.object_id].fourth_pattern = true;
				}

				lua_getglobal(clients[ev.object_id].L, "wander_boss");
				lua_pushnumber(clients[ev.object_id].L, ev.target_id);
				lua_pcall(clients[ev.object_id].L, 1, 0, 0);

				add_timer(ev.object_id, 100, EV_BOSS_CON, ev.target_id, clients[ev.object_id]._Room_Num);
				break;
			}
			case EV_BOSS_EYE: {
				int pl = ev.object_id;

				// 여기에서 보스가 중간으로 텔포 하도록 해줘야 함, 근데 보스방 센터 위치를 몰?루

				if (clients[pl].color_sequence[0] == 0) {

					clients[pl].boss_shield_trigger = true;

					send_cube(pl, clients[pl].x, clients[pl].y, clients[pl].z);

					for (auto& player : clients[pl].room_list) {
						if (player >= MAX_USER) continue;
						SC_BOSS_SHIELD_PACKET p;
						p.size = sizeof(SC_BOSS_SHIELD_PACKET);
						p.type = SC_BOSS_SHIELD;
						p.shield_hp = clients[pl].boss_shield;
						p.trigger = true;
						clients[player].do_send(&p);
					}

					srand((unsigned int)time(NULL));

					for (int i = 0; i < 3; i++) {
						clients[pl].color_sequence[i] = i + 1;
					}

					clients[pl].color_sequence[3] = 0;
				}

				for (auto& player : clients[pl].room_list) {
					if (player >= MAX_USER) continue;
					clients[player].send_boss_move(pl, clients[pl].x, clients[pl].y, clients[pl].z, clients[pl].degree,
						clients[pl].hp, clients[pl].char_state, clients[pl].color_sequence[clients[pl].eye_color], 0);
				}

				cout << clients[pl].color_sequence[clients[pl].eye_color] << endl;

				clients[pl].eye_color++;
				if (clients[pl].eye_color > 3) {
					clients[pl].eye_color = 0;
					break;
				}

				add_timer(pl, 3000, EV_BOSS_EYE, ev.target_id, clients[pl]._Room_Num);
				break;
			}
			case EV_BOSS_FIELD_ON: {
				for (int i = 0; i < 20; i++) {
					clients[ev.object_id].char_state = AN_ATTACK_1;
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER) continue;

						clients[pl]._sl.lock();
						if (clients[pl]._s_state != ST_INGAME) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						int field_num = ev.target_id * 20 + i;

						clients[pl].send_field_add(field_num, fields[field_num].x, fields[field_num].r, fields[field_num].z, AN_ATTACK_1, fields[field_num].type);

						float dec = abs(clients[ev.object_id].x - fields[field_num].x) + abs(clients[ev.object_id].z - fields[field_num].z);

						add_timer(ev.object_id, dec * 100, EV_STAGE2_FIRST_BOSS, field_num, clients[ev.object_id]._Room_Num);
					}
				}
				break;
			}
			case EV_BOSS_FIELD_OFF: {
				for (auto& pl : clients[ev.object_id].room_list) {
					if (pl >= MAX_USER) continue;

					clients[pl]._sl.lock();
					if (clients[pl]._s_state != ST_INGAME) {
						clients[pl]._sl.unlock();
						continue;
					}
					clients[pl]._sl.unlock();

					clients[pl].send_remove_object(ev.target_id, 2);
				}
				break;
			}
			case EV_BOSS_PLAYER_CON_OFF: {
				SC_BOSS_PLAYER_CON_PACKET p;
				p.type = SC_BOSS_PLAYER_CON;
				p.size = sizeof(SC_BOSS_PLAYER_CON_PACKET);
				p.con_num = ev.target_id;
				p.trigger = false;
				p.x = 0;
				p.y = 0;
				p.z = 0;
				switch (ev.target_id)
				{
				case 0: { // 이속 감소
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					
					break;
				}
				case 1: { // 조작 반전
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					
					break;
				}
				case 2: { // 마비
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					
					break;
				}
				case 3: { // 텔포
					// 좌표 업데이트 해야함
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						p.x = -35;
						p.y = 0;
						p.z = -175;

						clients[pl].do_send(&p);
					}
					
					break;
				}
				case 4: { // 회복
					for (auto& pl : clients[ev.object_id].room_list) {
						if (pl >= MAX_USER)continue;
						clients[pl]._sl.lock();
						if (clients[pl]._s_state == ST_FREE) {
							clients[pl]._sl.unlock();
							continue;
						}
						clients[pl]._sl.unlock();

						clients[pl].do_send(&p);
					}
					
					break;
				}
				default:
					break;
				}
				break;
			}
			case EV_COOLTIME: {
				clients[ev.object_id].cooltime = false;
				break;
			}
			default:
				break;
			}
		}
	}
}

void RESET_ROOM(int room_num)
{
	RESET_ROOM_NUM[room_num] = true;
}

void set_room()
{
	for (int i = 0; i < ROOM_NUM; i++) {
		RESET_ROOM_NUM[i] = false;
		RELOAD_LUA[i] = false;
	}
}
