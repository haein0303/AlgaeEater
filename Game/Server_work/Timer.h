#pragma once
#include "NPC.h"

enum EVENT_TYPE {
	EV_STAGE1_FIRST_BOSS, EV_STAGE1_SECOND_BOSS,
	EV_STAGE2_FIRST_BOSS, EV_STAGE3_BOSS,
	EV_NPC_CON, EV_BOSS_CON, EV_BOSS_EYE, EV_BOSS_FIELD_ON, EV_BOSS_FIELD_OFF,
	EV_BOSS_PLAYER_CON_OFF
};

struct TIMER_EVENT {
	int object_id;
	EVENT_TYPE ev;
	chrono::system_clock::time_point act_time;
	int target_id;
	int room_num;

	constexpr bool operator < (const TIMER_EVENT& _Left) const
	{
		return (act_time > _Left.act_time);
	}

};

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id, int room_num);

void do_timer();

void RESET_ROOM(int room_num);

void set_room();