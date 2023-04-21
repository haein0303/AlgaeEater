#pragma once


enum EVENT_TYPE { EV_MOVE, EV_RUSH, EV_CK, EV_CB, EV_UP, EV_RETURN, EV_NPC_CON };

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


void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id);

void do_timer();