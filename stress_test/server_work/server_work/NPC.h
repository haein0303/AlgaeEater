#pragma once

void initialize_npc();
void initialize_cube();

void send_cube(int c_id, float x, float y, float z);
void rush_npc(int player_id, int c_id);
void move_npc(int player_id, int c_id);
void return_npc(int c_id);