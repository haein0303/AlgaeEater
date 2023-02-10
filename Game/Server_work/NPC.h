#pragma once

void initialize_npc();
void initialize_cube();

void send_cube(int c_id, float x, float y, float z);
void move_npc(int npc_id);
void rush_npc(int player_id, int c_id);