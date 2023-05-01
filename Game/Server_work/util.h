#pragma once
void disconnect(int c_id);

int get_new_client_id();

void process_packet(int c_id, char* packet);

void disconnect(int c_id);

void do_worker();

void Update_Player(int c_id);

void Update_Npc();