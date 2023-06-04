#pragma once

void initialize_npc();
void initialize_cube();
void initialize_key();
void initialize_field();

void send_cube(int c_id, float x, float y, float z);
void rush_npc(int c_id, float t_x, float t_y);
void move_npc(int player_id, int c_id);
void return_npc(int c_id);
void wander_boss(int c_id);
void send_second_cube(int c_id, float x, float y, float z);