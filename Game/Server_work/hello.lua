my_id = 9999

AKI_ATK = 10
AKI_SKILL = 50
MIKA_ATK = 25
MIKA_SKILL = 100
AKI_HP = 100
AKI_MAX_HP = 100
MIKA_HP = 150
MIKA_MAX_HP = 150

distance = 30

BOSS1_ATK = 2
BOSS2_ATK = 2
BOSS3_ATK = 2

BOSS1_HP = 5000
BOSS2_HP = 5000
BOSS3_HP = 5000

BOSS1_MAX_HP = 5000
BOSS2_MAX_HP = 5000
BOSS3_MAX_HP = 5000

BOSS_FIRST_PATTERN = false;
BOSS_SECOND_PATTERN = false;
BOSS_THIRD_PATTERN = false;
BOSS_FOURTH_PATTERN = false;

function set_player( player_type )
	if (player_type == 0) then
	 API_PLAYER_SET(my_id, AKI_MAX_HP, AKI_HP, AKI_ATK, AKI_SKILL)
	elseif (player_type == 1) then
	 API_PLAYER_SET(my_id, MIKA_MAX_HP, MIKA_HP, MIKA_ATK, MIKA_SKILL)
	end
end

function set_boss( boss_type )
	if (boss_type == 3) then
	 API_BOSS_SET(my_id, BOSS1_MAX_HP, BOSS1_HP, BOSS1_ATK, BOSS_FIRST_PATTERN, BOSS_SECOND_PATTERN, BOSS_THIRD_PATTERN, BOSS_FOURTH_PATTERN)
	elseif (boss_type == 4) then
	 API_BOSS_SET(my_id, BOSS2_MAX_HP, BOSS2_HP, BOSS2_ATK, BOSS_FIRST_PATTERN, BOSS_SECOND_PATTERN, BOSS_THIRD_PATTERN, BOSS_FOURTH_PATTERN)
	elseif (boss_type == 5) then
	 API_BOSS_SET(my_id, BOSS3_MAX_HP, BOSS3_HP, BOSS3_ATK, BOSS_FIRST_PATTERN, BOSS_SECOND_PATTERN, BOSS_THIRD_PATTERN, BOSS_FOURTH_PATTERN)
	end
end

function set_object_id(id)
   my_id = id
end

function event_rush( player_id )
   t_x = API_get_x(player_id)
   t_z = API_get_z(player_id)

	API_Rush(my_id, t_x, t_z)
end

function wander_boss( player_id )
	player_x = API_get_x(player_id)
	player_z = API_get_z(player_id)
	npc_x = API_get_x(my_id)
	npc_z = API_get_z(my_id)

	if ( math.abs (player_x - npc_x) + math.abs (player_z - npc_z) <= distance) then
		API_Tracking (player_id, my_id)
	else
		API_Wander(my_id)
	end
end