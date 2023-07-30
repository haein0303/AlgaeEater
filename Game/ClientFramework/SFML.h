#pragma once
#include "Util.h"
#include <iostream>
#include "OBJECT.h"
#include <unordered_map>
#include "lobby_client.h"

class SFML
{
public:
	sf::TcpSocket socket;
	int myClientId = 0;
	
	int user_accept_counter = 0;
	unordered_map<int, int> user_map;

	int npc_accept_counter = 0;
	unordered_map<int, int> npc_map;

	int skill_accept_counter = 0;
	unordered_map<int, int> skill_map;
	
	int stage3_mini_boss_num = 0;
	int stage3_last_pass_user_count = 0;

	int ConnectServer(int PortNum,int Scene_select,int chat_type) //서버에 접속시 보내주는 부분
	{
		wcout.imbue(locale("korean"));
		sf::Socket::Status status = socket.connect("127.0.0.1", PortNum); // 59.14.135.128
		socket.setBlocking(false);

		if (status != sf::Socket::Done) {
			wcout << L"서버와 연결할 수 없습니다.\n";
			return -1;
			while (true);
		}

		CS_LOGIN_PACKET p;
		p.size = sizeof(CS_LOGIN_PACKET);
		p.type = CS_LOGIN;
		p.stage = Scene_select;
		p.character_type = chat_type;
		
		cout << "CONNECTSERVER char select : " << chat_type << endl;

		strcpy_s(p.name, "a");
		send_packet(&p);
		return 0;
	}

	int ConnectServer(int PortNum) 
	{
		wcout.imbue(locale("korean"));
		sf::Socket::Status status = socket.connect("127.0.0.1", PortNum); // 59.14.135.128
		socket.setBlocking(false);

		if (status != sf::Socket::Done) {
			wcout << L"서버와 연결할 수 없습니다.\n";
			return -1;
			while (true);
		}

		return 0;
	}

	void ReceiveServer(array<OBJECT, PLAYERMAX>& playerArr, OBJECT* npcArr, OBJECT* pillars_data, OBJECT& boss_obj, int& open_door_count) //서버에서 받는거, clientMain
	{
		char net_buf[BUF_SIZE];
		size_t	received;

		auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
		if (recv_result == sf::Socket::Error)
		{
			wcout << L"Recv 에러!";
			while (true);
		}
		if (recv_result != sf::Socket::NotReady)
			if (received > 0) process_data(net_buf, received, playerArr, npcArr, pillars_data, boss_obj, open_door_count);
	}

	void process_data(char* net_buf, size_t io_byte, array<OBJECT, PLAYERMAX>& playerArr, OBJECT* npcArr, OBJECT* pillars_data, OBJECT& boss_obj, int& open_door_count)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer, playerArr, npcArr, pillars_data, boss_obj, open_door_count);
				ptr += in_packet_size - saved_packet_size;
				io_byte -= in_packet_size - saved_packet_size;
				in_packet_size = 0;
				saved_packet_size = 0;
			}
			else {
				memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
				saved_packet_size += io_byte;
				io_byte = 0;
			}
		}
	}

	int getNPCid(int id_from_server) {
		if (npc_map.count(id_from_server) == 0) {
			cout << "getNPCid is fail" << endl;
			return -1;
		}
		return npc_map.find(id_from_server)->second;		
	}



	int getUSERid(int id_from_server) {
		if (user_map.count(id_from_server) == 0) {
			cout << "getUSERid is fail" << endl;
			return -1;
		}
		return user_map.find(id_from_server)->second;
	}

	int getSkillid(int id_from_server) {
		if (skill_map.count(id_from_server) == 0) {
			cout << "getNPCid is fail" << endl;
			return -1;
		}
		return skill_map.find(id_from_server)->second;
	}

	//있으면 기존 값 반환 없으면 새로 만들어서 반환
	int addSkillid(const int& id_from_server) {
		if (skill_map.count(id_from_server) != 0) {
			return skill_map.find(id_from_server)->second;
		}

		skill_map.insert({ id_from_server,skill_accept_counter });
		return skill_accept_counter++;
	}


	//서버에서 데이터 받을때(패킷종류별로 무슨 작업 할건지 ex: 이동 패킷, 로그인 패킷 how to 처리)
	void ProcessPacket(char* ptr, array<OBJECT, PLAYERMAX>& playerArr, OBJECT* npcArr, OBJECT* pillars_data, OBJECT& boss_obj, int& open_door_count)
	{
		static bool first_time = true;
		switch (ptr[1])
		{
		case SC_LOGIN_OK:
		{
			SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(ptr);

			user_map.insert({ packet->id,user_accept_counter });
			cout << myClientId << " : id : " << packet->id << " || user_accept_counter : " << user_accept_counter << " : " << user_map.count(packet->id) << endl;
			playerArr[myClientId]._on = true;
			playerArr[myClientId]._my_server_id = packet->id;
			playerArr[myClientId]._transform.x = packet->x;
			playerArr[myClientId]._transform.y = packet->y;
			playerArr[myClientId]._transform.z = packet->z;
			playerArr[myClientId]._degree = packet->degree;
			user_accept_counter++;
			break;
		}
		case SC_ADD_OBJECT:
		{
			SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
			int id = my_packet->id;

			if (id < MAX_USER) { // MAX_USER로 교체
				user_map.insert({ id,user_accept_counter });
				cout << "id : " << id << " || user_accept_counter : " << user_accept_counter << " : " << user_map.count(id) << endl;
				playerArr[user_accept_counter]._on = true;
				playerArr[user_accept_counter]._my_server_id = id;
				playerArr[user_accept_counter]._transform.x = my_packet->x;
				playerArr[user_accept_counter]._transform.y = my_packet->y;
				playerArr[user_accept_counter]._transform.z = my_packet->z;
				playerArr[user_accept_counter]._degree = my_packet->degree;
				switch (my_packet->object_type) {
				case SESSION_TYPE::TY_PLAYER_AKI:
					playerArr[user_accept_counter]._character_num = 0;
					break;
				case SESSION_TYPE::TY_PLAYER_MIKA:
					playerArr[user_accept_counter]._character_num = 1;
					break;
				default:
					playerArr[user_accept_counter]._character_num = 0;
				}
				cout << "SC_ADD_OBJECT : " << my_packet->object_type << endl;
				playerArr[user_accept_counter]._prev_degree = playerArr[user_accept_counter]._degree;
				playerArr[user_accept_counter]._prev_transform = playerArr[user_accept_counter]._transform;
				playerArr[user_accept_counter]._prev_delta_time = playerArr[user_accept_counter]._delta_time;
				//델타타임은 초기화를 합시다.
				playerArr[user_accept_counter]._delta_time = 0.f;

				user_accept_counter++;
			}
			else
			{
				npc_map.insert({ id,npc_accept_counter });
				cout << "id : " << id << " || npc_accept_counter : " << npc_accept_counter << " : " << npc_map.count(id) << endl;
				npcArr[npc_accept_counter]._on = true;
				npcArr[npc_accept_counter]._my_server_id = id;
				npcArr[npc_accept_counter]._transform.x = my_packet->x;
				npcArr[npc_accept_counter]._transform.y = my_packet->y;
				npcArr[npc_accept_counter]._transform.z = my_packet->z;
				npcArr[npc_accept_counter]._degree = my_packet->degree;
				npcArr[npc_accept_counter]._hp = my_packet->hp;
				npcArr[npc_accept_counter]._max_hp = my_packet->hp;
				npcArr[npc_accept_counter]._object_type = my_packet->object_type;
				if (my_packet->object_type == TY_BOSS_1) {
					stage3_mini_boss_num = npc_accept_counter;
				}

				npcArr[npc_accept_counter]._prev_degree = npcArr[npc_accept_counter]._degree;
				npcArr[npc_accept_counter]._prev_transform = npcArr[npc_accept_counter]._transform;
				npcArr[npc_accept_counter]._prev_delta_time = npcArr[npc_accept_counter]._delta_time;
				//델타타임은 초기화를 합시다.
				npcArr[npc_accept_counter]._delta_time = 0.f;

				npc_accept_counter++;
			}

			break;
		}
		case SC_ADD_BOSS: {
			SC_ADD_BOSS_PACKET* my_packet = reinterpret_cast<SC_ADD_BOSS_PACKET*>(ptr);
			int id = my_packet->id;
			cout << "BOSS ID : " << id << endl;
			boss_obj._on = true;
			boss_obj._my_server_id = id;
			boss_obj._transform.x = my_packet->x;
			boss_obj._transform.y = my_packet->y;
			boss_obj._transform.z = my_packet->z;
			boss_obj._degree = my_packet->degree;
			boss_obj._max_hp = BOSS_HP[0];
			boss_obj._hp = my_packet->hp;
			boss_obj._animation_state = my_packet->char_state;

			boss_obj._prev_degree = boss_obj._degree;
			boss_obj._prev_transform = boss_obj._transform;
			boss_obj._prev_delta_time = boss_obj._delta_time;
			//델타타임은 초기화를 합시다.
			boss_obj._delta_time = 0.f;


			break;
		}
		case SC_MOVE_BOSS: {
			SC_MOVE_BOSS_PACKET* my_packet = reinterpret_cast<SC_MOVE_BOSS_PACKET*>(ptr);
			int id = my_packet->id;

			boss_obj._prev_degree = boss_obj._degree;
			boss_obj._prev_transform = boss_obj._transform;
			boss_obj._prev_delta_time = boss_obj._delta_time;
			//델타타임은 초기화를 합시다.
			boss_obj._delta_time = 0.f;

			boss_obj._on = true;
			boss_obj._my_server_id = id;
			boss_obj._transform.x = my_packet->x;
			boss_obj._transform.y = my_packet->y;
			boss_obj._transform.z = my_packet->z;
			boss_obj._degree = my_packet->degree;
			boss_obj._hp = my_packet->hp;
			boss_obj._animation_state = my_packet->char_state;
			if (my_packet->char_state == AnimationOrder::BossDown)
				cout << "boss down" << endl;
			boss_obj._eye_color = my_packet->eye_color; // 0,1,2,3,4
			//cout << boss_obj._eye_color << endl;
			break;
		}
		case SC_MOVE_OBJECT:
		{
			SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
			int id = my_packet->id;
			if (id < MAX_USER) {// MAX_USER로 교체
				id = getUSERid(my_packet->id);

				playerArr[id]._hp = my_packet->hp;

				if (myClientId != id) {
					//이전꺼 저장중
					playerArr[id]._prev_degree = playerArr[id]._degree;
					playerArr[id]._prev_transform = playerArr[id]._transform;
					playerArr[id]._prev_delta_time = playerArr[id]._delta_time;
					//델타타임은 초기화를 합시다.
					playerArr[id]._delta_time = 0.f;

					playerArr[id]._transform.x = my_packet->x;
					playerArr[id]._transform.y = my_packet->y;
					playerArr[id]._transform.z = my_packet->z;
					playerArr[id]._degree = my_packet->degree;
					playerArr[id]._animation_state = my_packet->char_state;

					if (playerArr[id]._animation_state0 != playerArr[id]._animation_state) {
						playerArr[id]._animation_time_pos = 0.f;
						playerArr[id]._animation_state0 = playerArr[id]._animation_state;
					}
				}

			}
			else {
				id = getNPCid(my_packet->id);

				npcArr[id]._prev_degree = npcArr[id]._degree;
				npcArr[id]._prev_transform = npcArr[id]._transform;
				npcArr[id]._prev_delta_time = npcArr[id]._delta_time;
				//델타타임은 초기화를 합시다.
				npcArr[id]._delta_time = 0.f;


				npcArr[id]._transform.x = my_packet->x;
				npcArr[id]._transform.y = my_packet->y;
				npcArr[id]._transform.z = my_packet->z;
				npcArr[id]._degree = my_packet->degree;
				npcArr[id]._hp = my_packet->hp;
				npcArr[id]._animation_state = my_packet->char_state;
			}
			break;
		}
		case SC_REMOVE_OBJECT:
		{
			SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
			if (my_packet->ob_type == 0) { //0은 플레이어 1은 기둥
				int id = getUSERid(my_packet->id);
				playerArr[id]._on = false;
			}

			if (my_packet->ob_type == 1) { //0은 플레이어 1은 기둥
				int id = my_packet->id;
				pillars_data[id]._pillar_count = 0;
			}

			if (my_packet->ob_type == 2) { //0은 플레이어 1은 기둥 2는 장판
				cout << "skill off : " << my_packet->id << endl;
				boss_obj.boss2_skill_vec[getSkillid(my_packet->id)].isOn = false;
			}

			break;
		}
		case SC_ADD_CUBE:
		{
			SC_ADD_CUBE_PACKET* my_packet = reinterpret_cast<SC_ADD_CUBE_PACKET*>(ptr);
			int id = my_packet->id;
			pillars_data[id]._on = true;
			pillars_data[id]._transform.x = my_packet->x;
			pillars_data[id]._transform.y = my_packet->y;
			pillars_data[id]._transform.z = my_packet->z;
			pillars_data[id]._pillar_color = my_packet->color;
			pillars_data[id]._degree = my_packet->degree;
			pillars_data[id]._pillar_count = my_packet->hp;
			cout << "pillar hp " << pillars_data[id]._pillar_count << endl;

			break;
		}
		case SC_KEY:
		{
			/*SC_KEY_PACKET* my_packet = reinterpret_cast<SC_KEY_PACKET*>(ptr);
			int id = my_packet->color;
			key_data[id]._on = true;
			key_data[id]._key = id;
			key_data[id]._transform.x = my_packet->x;
			key_data[id]._transform.y = my_packet->y;
			key_data[id]._transform.z = my_packet->z;
			cout << "SEND KEY : [" << id << "]" << endl;*/

			break;
		}
		case SC_DOOR: {
			open_door_count++;

			break;
		}
		case SC_BOSS_SKILL_START: {
			SC_BOSS_SKILL_START_PACKET* packet = reinterpret_cast<SC_BOSS_SKILL_START_PACKET*>(ptr);
			cout << "pre SKILL START : " << packet->fd_id << endl;
			int tmp = addSkillid(packet->fd_id);
			cout << "SKILL START : " << tmp << endl;
			Boss2SkillData boss2_skill;
			boss2_skill.my_server_id = packet->fd_id;
			boss2_skill.pos.x = packet->x;
			boss2_skill.pos.y = 0.01f;
			boss2_skill.pos.z = packet->z;
			boss2_skill.scale = packet->r;
			boss2_skill.type = packet->fd_type;
			boss2_skill.atk_time = 0.f;
			cout << "packet->fd_type : " << packet->fd_type << endl;
			if (boss2_skill.type == 0 || boss2_skill.type == 1)
				boss2_skill.animation_count = 0;
			else if (boss2_skill.type == 2)
				boss2_skill.animation_count = 5;
			
			//0 : 원형 회복 / 1 : 사각 기본 공격 / 2 : 사각 아픈 공격
			boss2_skill.isOn = false;

			Boss2SkillData boss2_skill_fire_data;
			boss2_skill_fire_data.my_server_id = packet->fd_id;
			boss2_skill_fire_data.pos.x = boss_obj._transform.x + 3.5f * sinf((boss_obj._degree - 150.f) * XM_PI / 180.f);
			boss2_skill_fire_data.pos.y = boss_obj._transform.y + 3.f;
			boss2_skill_fire_data.pos.z = boss_obj._transform.z + 3.5f * cosf((boss_obj._degree - 150.f) * XM_PI / 180.f);
			boss2_skill_fire_data.start_pos = boss2_skill_fire_data.pos;
			boss2_skill_fire_data.scale = 1.f;
			boss2_skill_fire_data.animation_count = 0;
			boss2_skill_fire_data.isOn = true;
			
			boss2_skill_fire_data.delta_time = 0.f;
			
			
			//연산 파트
			{
				float dec = abs(boss2_skill.pos.x - boss2_skill_fire_data.pos.x) +
					abs(boss2_skill.pos.z - boss2_skill_fire_data.pos.z);

				boss2_skill_fire_data.goal_time = dec * 100;
				boss2_skill_fire_data.goal_time /= 1000; // 서버는 ms가 기준이지만 클라는 s가 기준

				boss2_skill_fire_data.delta_pos.x = boss2_skill.pos.x - boss2_skill_fire_data.pos.x;
				boss2_skill_fire_data.delta_pos.y = boss2_skill.pos.y - boss2_skill_fire_data.pos.y;
				boss2_skill_fire_data.delta_pos.z = boss2_skill.pos.z - boss2_skill_fire_data.pos.z;

			}
			

			//혹시 더 많이 보낼 수 있어
			if (tmp > boss_obj.boss2_skill_vec.size()) { //멀쓰이기때문에 상당하게 위험
				boss_obj.boss2_skill_vec.emplace_back(boss2_skill);
				boss_obj.boss2_skill_fire_vec.emplace_back(boss2_skill_fire_data);
			}
			else {
				boss_obj.boss2_skill_vec[tmp] = boss2_skill;
				boss_obj.boss2_skill_fire_vec[tmp] = boss2_skill_fire_data;
				//cout << "SKILL START - point : " << tmp << endl;
			}			

			break;
		}
		case SC_BOSS_SKILL_END: {
			SC_BOSS_SKILL_END_PACKET* packet = reinterpret_cast<SC_BOSS_SKILL_END_PACKET*>(ptr);

			boss_obj.boss2_skill_vec[getSkillid(packet->fd_id)].isOn = true;
			boss_obj.boss2_skill_fire_vec[getSkillid(packet->fd_id)].isOn = false;

			boss_obj._boss_skill_count++;
			break;
		}
		case SC_BOSS_SHIELD: {
			SC_BOSS_SHIELD_PACKET* packet = reinterpret_cast<SC_BOSS_SHIELD_PACKET*>(ptr);
			if (boss_obj._shield_on == false) {
				boss_obj._shield_on = packet->trigger;
				boss_obj._max_shield = packet->shield_hp;
				cout << "MAX SHIELD : " << boss_obj._max_shield << endl;
			}

			boss_obj._shield = packet->shield_hp;
			cout << "MAX SHIELD : " << boss_obj._max_shield;
			cout << "   SHIELD : " << boss_obj._shield << endl;

			break;
		}
		case SC_BOSS_RUSH_TARGET: {
			SC_BOSS_RUSH_TARGET_PACKET* packet = reinterpret_cast<SC_BOSS_RUSH_TARGET_PACKET*>(ptr);
			boss_obj._stage1_target_alert_on = packet->trigger;
			boss_obj._stage1_target_id = packet->target_id;
		}
		break;

		case SC_BOSS_PLAYER_CON: {
			SC_BOSS_PLAYER_CON_PACKET* packet = reinterpret_cast<SC_BOSS_PLAYER_CON_PACKET*>(ptr);
			boss_obj._stage3_boss_con = packet->con_num;
			boss_obj._stage3_boss_on = packet->trigger;
			playerArr[myClientId]._stage3_boss_con = packet->con_num;
			playerArr[myClientId]._stage3_boss_on = packet->trigger;
			
			if (packet->con_num == 3) {
				playerArr[myClientId]._transform.x = packet->x;
				playerArr[myClientId]._transform.y = packet->y;
				playerArr[myClientId]._transform.z = packet->z;
			}


		}break;

		case SC_GAME_END: {
			SC_GAME_END_PACKET* packet = reinterpret_cast<SC_GAME_END_PACKET*>(ptr);
			boss_obj._game_clear = true;
			boss_obj._clear_type = packet->e_type;

			break;
		}
		case SC_NPC_TARGET: {
			SC_NPC_TARGET_PACKET* my_packet = reinterpret_cast<SC_NPC_TARGET_PACKET*>(ptr);
			int id = my_packet->npc_id;
			if (id >= MAX_USER) {// MAX_USER로 교체
				id = getNPCid(my_packet->npc_id);
				if(id == -1)
					boss_obj._who_target = my_packet->player_id;
				else
					npcArr[id]._who_target = my_packet->player_id;
			}

			break;
		}
		case SC_BROAD_CAST: {
			SC_BROAD_CAST_PACKET* my_packet = reinterpret_cast<SC_BROAD_CAST_PACKET*>(ptr);
			switch (my_packet->pri) {
			case 31:
				cout << "첫번째문 오픈이요오" << endl;
				open_door_count = 1;
				break;
			case 33:
				cout << "세번째문 오픈이요오" << endl;
				open_door_count = 3;
				break;
			case 34:
				stage3_last_pass_user_count++;
				cout << "통과인원 " << stage3_last_pass_user_count << endl;
				if (stage3_last_pass_user_count >= user_accept_counter) {
					open_door_count = 4;
				}
				break;
			}
		}
		break;
		default:
			printf("Unknown PACKET type [%d]\n", ptr[1]);
		}
	}

	//서버로 패킷 보내줄 때
	void send_packet(void* packet)
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(packet);
		size_t sent = 0;
		socket.send(packet, p[0], sent);
	}





	void process_data(char* net_buf, size_t io_byte, int& data,int &scene_num)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer, data, scene_num);
				ptr += in_packet_size - saved_packet_size;
				io_byte -= in_packet_size - saved_packet_size;
				in_packet_size = 0;
				saved_packet_size = 0;
			}
			else {
				memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
				saved_packet_size += io_byte;
				io_byte = 0;
			}
		}
	}

	void ReceiveServer(int &data,int &scene_num) //서버에서 받는거, clientMain
	{
		char net_buf[BUF_SIZE];
		size_t	received;

		auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
		if (recv_result == sf::Socket::Error)
		{
			wcout << L"Recv 에러!";
			while (true);
		}
		if (recv_result != sf::Socket::NotReady)
			if (received > 0) process_data(net_buf, received, data, scene_num);
	}



	void ProcessPacket(char* ptr, int& data,int&scene_num) {
		switch (ptr[1])
		{
			case LSC_LOGIN_OK: 
			{
				LSC_LOGIN_OK_PACKET* packet = reinterpret_cast<LSC_LOGIN_OK_PACKET*>(ptr);
				scene_num = SCENE_STATE::READY;


				cout << "로그인 OK" << endl;
			}
			break;

			case LSC_CONGAME: 
			{
				LSC_CONGAME_PACKET* packet = reinterpret_cast<LSC_CONGAME_PACKET*>(ptr);
				data = -1;
			}
			break;

			case LSC_LOGIN_FAIL:
			{
				LSC_LOGIN_FAIL_PACKET* packet = reinterpret_cast<LSC_LOGIN_FAIL_PACKET*>(ptr);
				cout << "로그인 FAIL" << endl;
			}
			break;

			case LSC_JOIN_OK:
			{
				LSC_LOGIN_FAIL_PACKET* packet = reinterpret_cast<LSC_LOGIN_FAIL_PACKET*>(ptr);
				scene_num = SCENE_STATE::LOG_IN;
				cout << "LSC_JOIN_OK" << endl;
			}
			break;

			case LSC_JOIN_FAIL:
			{
				LSC_LOGIN_FAIL_PACKET* packet = reinterpret_cast<LSC_LOGIN_FAIL_PACKET*>(ptr);
				cout << "LSC_JOIN_FAIL" << endl;
			}
			break;
		}
	}

};