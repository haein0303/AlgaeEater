
#include "Util.h"
#include "DxEngine.h"
#include "OBJECT.h"
#include "MESH.h"
#include "SFML.h"

const float boss2_atk_delta_time_set = 0.4f;

void OBJECT::Init(WindowInfo windowInfo)
{
}

void OBJECT::FixedUpdate(WindowInfo windowInfo, bool isActive)
{
}

void OBJECT::Update(WindowInfo windowInfo, bool isActive)
{
}

void OBJECT::Draw(WindowInfo windowInfo, int)
{
}

//자기 자신만 모든 보스 장판에 대하여 비교
bool boss2_skill_checker(OBJECT& player, OBJECT& boss, shared_ptr<SFML> networkPtr)
{
	bool atked = false;

	for (auto& atk : boss.boss2_skill_vec) {
		if (atk.isOn == false) continue;
		
		bool toggle = false;
		switch (atk.type) {
		case 0:
		{

			float tmp = (atk.pos.x - player._transform.x) * (atk.pos.x - player._transform.x) +
				(atk.pos.z - player._transform.z) * (atk.pos.z - player._transform.z);
			if (tmp < atk.scale * atk.scale) {
				toggle = true;
				atked = true;
			}
		}
		break;
		case 1:case 2:case 11:
		{

			if (player._transform.x >= atk.pos.x - atk.scale &&
				player._transform.x <= atk.pos.x + atk.scale &&
				player._transform.z >= atk.pos.z - atk.scale &&
				player._transform.z <= atk.pos.z + atk.scale
				)
			{
				toggle = true;
				atked = true;
			}

		}
		break;
		}

		if (atk.atk_time < boss2_atk_delta_time_set) {
			continue;
		}
		else {
			atk.atk_time = 0.f;
		}
				
		if (toggle) {
			
			CS_OBJECT_COLLISION_PACKET p;
			p.size = sizeof(p);
			p.type = CS_OBJECT_COLLISION;
			p.attacker_id = atk.my_server_id;
			p.target_id = player._my_server_id;
			p.object_type = 1;
			networkPtr->send_packet(&p);
		}	
		
	}
	//cout << atked << endl;
	return atked;

}

void boss2_skill_logical_update(OBJECT& boss, shared_ptr<Timer> timer)
{
	for (auto& atk : boss.boss2_skill_vec) {
		if (atk.isOn == false) continue;

		atk.atk_time += timer->_deltaTime;
	}
}
