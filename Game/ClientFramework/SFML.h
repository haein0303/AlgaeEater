#pragma once
#include "Util.h"
#include <iostream>
#include "OBJECT.h"

class SFML
{
public:
	sf::TcpSocket socket;
	int myClientId = 0;

	int ConnectServer(int PortNum) //서버에 접속시 보내주는 부분
	{
		wcout.imbue(locale("korean"));
		sf::Socket::Status status = socket.connect("127.0.0.1", PortNum);
		socket.setBlocking(false);

		if (status != sf::Socket::Done) {
			wcout << L"서버와 연결할 수 없습니다.\n";
			return -1;
			while (true);
		}

		CS_LOGIN_PACKET p;
		p.size = sizeof(CS_LOGIN_PACKET);
		p.type = CS_LOGIN;
		strcpy_s(p.name, "a");
		send_packet(&p);
		return 0;
	}

	void ReceiveServer(OBJECT* playerArr, OBJECT* npcArr, OBJECT* cubeArr) //서버에서 받는거, clientMain
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
			if (received > 0) process_data(net_buf, received, playerArr, npcArr, cubeArr);
	}

	void process_data(char* net_buf, size_t io_byte, OBJECT* playerArr, OBJECT* npcArr, OBJECT* cubeArr)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer, playerArr, npcArr, cubeArr);
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



	//서버에서 데이터 받을때(패킷종류별로 무슨 작업 할건지 ex: 이동 패킷, 로그인 패킷 how to 처리)
	void ProcessPacket(char* ptr, OBJECT* playerArr, OBJECT* npcArr, OBJECT* cubeArr)
	{
		static bool first_time = true;
		switch (ptr[1])
		{
		case SC_LOGIN_OK:
		{
			SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(ptr);
			myClientId = packet->id;
			playerArr[myClientId]._on = true;
			playerArr[myClientId]._transform.x = packet->x;
			playerArr[myClientId]._transform.y = packet->y;
			playerArr[myClientId]._transform.z = packet->z;
			playerArr[myClientId]._degree = packet->degree;

			break;
		}
		case SC_ADD_OBJECT:
		{
			SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
			int id = my_packet->id;
			if (id < PLAYERMAX) {
				playerArr[id]._on = true;
				playerArr[id]._transform.x = my_packet->x;
				playerArr[id]._transform.y = my_packet->y;
				playerArr[id]._transform.z = my_packet->z;
				playerArr[id]._degree = my_packet->degree;
			}
			else if (id >= PLAYERMAX)
			{
				npcArr[id - PLAYERMAX]._on = true;
				npcArr[id - PLAYERMAX]._transform.x = my_packet->x;
				npcArr[id - PLAYERMAX]._transform.y = my_packet->y;
				npcArr[id - PLAYERMAX]._transform.z = my_packet->z;
				npcArr[id - PLAYERMAX]._degree = my_packet->degree;
			}

			break;
		}
		case SC_MOVE_OBJECT:
		{
			SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
			int id = my_packet->id;
			if (id < PLAYERMAX)
			{
				playerArr[id]._hp = my_packet->hp;
			}
			else if (id < PLAYERMAX && myClientId != id) {
				playerArr[id]._transform.x = my_packet->x;
				playerArr[id]._transform.y = my_packet->y;
				playerArr[id]._transform.z = my_packet->z;
				playerArr[id]._degree = my_packet->degree;
				playerArr[id]._animation_state = my_packet->char_state;
			}
			else if (id >= PLAYERMAX)
			{
				npcArr[id - PLAYERMAX]._transform.x = my_packet->x;
				npcArr[id - PLAYERMAX]._transform.y = my_packet->y;
				npcArr[id - PLAYERMAX]._transform.z = my_packet->z;
				npcArr[id - PLAYERMAX]._degree = my_packet->degree;
				npcArr[id - PLAYERMAX]._hp = my_packet->hp;
				npcArr[id - PLAYERMAX]._animation_state = my_packet->char_state;
			}
			
			break;
		}
		case SC_REMOVE_OBJECT:
		{
			SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
			int id = my_packet->id;
			playerArr[id]._on = false;
			break;
		}
		case SC_ADD_CUBE:
		{
			SC_ADD_CUBE_PACKET* my_packet = reinterpret_cast<SC_ADD_CUBE_PACKET*>(ptr);
			int id = my_packet->id;
			cubeArr[id]._on = true;
			cubeArr[id]._transform.x = my_packet->x;
			cubeArr[id]._transform.y = my_packet->y;
			cubeArr[id]._transform.z = my_packet->z;
			cubeArr[id]._degree = my_packet->degree;

			break;
		}
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


	/*
	* 20230405 임해인 추가
	* 
	* 아래로는 Lobby용 오버로딩된 함수들이라네~ 
	*/


	void process_data(char* net_buf, size_t io_byte, int& data)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer, data);
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

	void ReceiveServer(int &data) //서버에서 받는거, clientMain
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
			if (received > 0) process_data(net_buf, received, data);
	}



	void ProcessPacket(char* ptr, int& data) {
		switch (ptr[1])
		{
		case LSC_LOGIN_OK: 
		{
			LSC_LOGIN_OK_PACKET* packet = reinterpret_cast<LSC_LOGIN_OK_PACKET*>(ptr);
		}
		break;
		case LSC_CONGAME: 
		{
			LSC_CONGAME_PACKET* packet = reinterpret_cast<LSC_CONGAME_PACKET*>(ptr);
			data = -1;
		}
		break;
		}
	}

};