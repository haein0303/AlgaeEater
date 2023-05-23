my_id = 9999

function set_object_id(id)
   my_id = id
end

function event_rush( player_id )
   t_x = API_get_x(player_id)
   t_z = API_get_z(player_id)

	API_Rush(my_id, t_x, t_z)
end

function create_cube( target_id )
		API_Cube(target_id)
end

function tracking_player( player_id )
	player_x = API_get_x(player_id)
	player_z = API_get_z(player_id)
	npc_start_x = API_get_npc_x(my_id)
	npc_start_z = API_get_npc_z(my_id)
	npc_x = API_get_x(my_id)
	npc_z = API_get_z(my_id)

	if ( math.abs (player_x - npc_start_x) + math.abs (player_z - npc_start_z) <= 15) then
		API_Tracking (player_id, my_id)
	else
		if (npc_start_x ~= npc_x or npc_start_z ~= npc_z) then
			API_Return (my_id)
		end
	end
end

function wander_boss( player_id )
	player_x = API_get_x(player_id)
	player_z = API_get_z(player_id)
	npc_start_x = API_get_npc_x(my_id)
	npc_start_z = API_get_npc_z(my_id)
	npc_x = API_get_x(my_id)
	npc_z = API_get_z(my_id)

	if ( math.abs (player_x - npc_x) + math.abs (player_z - npc_z) <= 30) then
		API_Tracking (player_id, my_id)
	else
		API_Wander(my_id)
	end
end