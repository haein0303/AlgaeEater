my_id = 9999

function set_object_id(id)
   my_id = id
end

function event_rush( player_id )
   state = API_get_state(player_id)

   if (state == 2) then
		API_Rush(player_id, my_id)
   end
end

function create_cube( target_id )
		API_Cube(target_id)
end

function tracking_player( player_id )
	player_x = API_get_x(player_id)
	player_z = API_get_z(player_id)
	npc_x = API_get_x(player_id)
	npc_z = API_get_z(player_id)


end