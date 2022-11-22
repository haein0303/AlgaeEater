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