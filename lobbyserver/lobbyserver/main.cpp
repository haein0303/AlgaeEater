#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <random>
#include <windows.h>  
#include <sqlext.h>  
#include <locale.h>
//#include "protocol.h"
#include "../../Game/Server_work/protocol.h"

#define UNICODE  
#include <sqlext.h>  
#include <string>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;

#define DATA_LEN 30

int add_user(const char* id, const char* pw);

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_NPC_RUSH, OP_CREATE_CUBE, OP_UPDATE };

enum SESSION_STATE { ST_FREE, ST_ACCEPTED, ST_INGAME };

enum ROOM_STATE { RO_READY, RO_INGAME };

class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	int target_id;

	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		::ZeroMemory(&_over, sizeof(_over));
	}

	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		::ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class SESSION {
	OVER_EXP _recv_over;
public:
	mutex	_sl;
	SESSION_STATE _s_state;
	int _id;
	SOCKET _socket;
	char	_name[NAME_SIZE];
	int		_state;
	int		_prev_remain;
	int		_stage;

public:
	SESSION()
	{
		_id = -1;
		_socket = 0;
		_name[0] = 0;
		_s_state = ST_FREE;
		_state = 0;
		_prev_remain = 0;
		_stage = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		::memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
	void send_login_ok_packet(int c_id);
};

void SESSION::send_login_ok_packet(int c_id)
{
	SC_LOGIN_OK_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_LOGIN_OK_PACKET);
	p.type = SC_LOGIN_OK;
	do_send(&p);
}

class USER_DATA {
public:
	char user_id[NAME_SIZE];
	char user_passward[NAME_SIZE];
	int user_level;
};

array<SESSION, MAX_USER> clients;
array<USER_DATA, MAX_USER> user_datas;

HANDLE g_h_iocp;
SOCKET g_s_socket;
SOCKET ss_socket;
list<int> test_match_list;
list<int> stage1_match_list;
list<int> stage2_match_list;
list<int> stage3_match_list;

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i]._sl.lock();
		if (clients[i]._s_state == ST_FREE) {
			clients[i]._s_state = ST_ACCEPTED;
			clients[i]._sl.unlock();
			return i;
		}
		clients[i]._sl.unlock();
	}
	return -1;
}

void disconnect(int c_id)
{
	clients[c_id]._sl.lock();
	if (clients[c_id]._s_state == ST_FREE) {
		clients[c_id]._sl.unlock();
		return;
	}
	closesocket(clients[c_id]._socket);
	clients[c_id]._s_state = ST_FREE;
	clients[c_id]._sl.unlock();
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case LCS_LOGIN: {
		int sdk = 0;
		clients[c_id]._sl.lock();
		if (clients[c_id]._s_state == ST_FREE) {
			clients[c_id]._sl.unlock();
			break;
		}
		if (clients[c_id]._s_state == ST_INGAME) {
			clients[c_id]._sl.unlock();
			disconnect(c_id);
			break;
		}

		LCS_LOGIN_PACKET* pl = reinterpret_cast<LCS_LOGIN_PACKET*>(packet);

		cout << "id : " << pl->id << "  pass : " << pl->passward << endl;
		cout << user_datas[0].user_id << user_datas[0].user_passward << endl;

		cout << "클라에서 보내준거 " << strlen(pl->id) << "  서버에서 저장된거" << strlen(user_datas[0].user_id) << endl;

		for (int i = 0; i < MAX_USER; i++) {
			if (sizeof(user_datas[i].user_id) == 0) continue;
			if (strcmp(user_datas[i].user_id, pl->id) == 0) {
				if (strcmp(user_datas[i].user_passward, pl->passward) == 0) {
					LSC_LOGIN_OK_PACKET p;
					p.id = c_id;
					p.size = sizeof(LSC_LOGIN_OK_PACKET);
					p.type = LSC_LOGIN_OK;
					p.level = user_datas[i].user_level;

					clients[c_id].do_send(&p);
					clients[c_id]._s_state = ST_INGAME;
					clients[c_id]._sl.unlock();
					cout << "클라 접속 확인" << endl;
					break;
				}
				else {
					LSC_LOGIN_FAIL_PACKET p;
					p.size = sizeof(LSC_LOGIN_FAIL_PACKET);
					p.type = LSC_LOGIN_FAIL;

					clients[c_id].do_send(&p);
					clients[c_id]._sl.unlock();
					cout << "클라 접속 실패" << endl;
					break;
				}
			}
			else sdk++;
		}

		if (sdk >= MAX_USER) {
			LSC_LOGIN_FAIL_PACKET p;
			p.size = sizeof(LSC_LOGIN_FAIL_PACKET);
			p.type = LSC_LOGIN_FAIL;

			clients[c_id].do_send(&p);
			clients[c_id]._sl.unlock();
			cout << "클라 접속 실패" << endl;
			break;
		}
		
		break;
	}
	case LCS_MATCH: {
		LCS_MATCH_PACKET* p = reinterpret_cast<LCS_MATCH_PACKET*>(packet);
		clients[c_id]._state = 1;
		clients[c_id]._stage = p->stage;

		cout << "match : " << p->stage << endl;

		// p->stage = 0이면 테스트, 1이면 스테이지 1
		switch (clients[c_id]._stage)
		{
			case 0:
				test_match_list.push_back(c_id);
				break;
			case 1:
				stage1_match_list.push_back(c_id);
				break;
			case 2:
				stage2_match_list.push_back(c_id);
				break;
			case 3:
				stage3_match_list.push_back(c_id);
				break;
			default:
				break;
		}
		break;
	}
	case LCS_OUT: {
		clients[c_id]._state = 0;
		switch (clients[c_id]._stage)
		{
		case 0:
			test_match_list.remove(c_id);
			break;
		case 1:
			stage1_match_list.remove(c_id);
			break;
		case 2:
			stage2_match_list.remove(c_id);
			break;
		case 3:
			stage3_match_list.remove(c_id);
			break;
		default:
			break;
		}
		break;
	}
	case LCS_JOIN: {
		LCS_JOIN_PACKET* p = reinterpret_cast<LCS_JOIN_PACKET*>(packet);
		p->id;
		p->passward;

		if (1 == add_user(p->id, p->passward)) {
			// 성공
			cout << "생성 성공" << endl;
			LSC_JOIN_OK_PACKET pac;
			pac.size = sizeof(LSC_JOIN_OK_PACKET);
			pac.type = LSC_JOIN_OK;

			for (int i = 0; i < MAX_USER; i++) {
				if (user_datas[i].user_id[0] == 0) {
					strcpy(user_datas[i].user_id, p->id);
					strcpy(user_datas[i].user_passward, p->passward);
					user_datas[i].user_level = 1;
					break;
				}
			}

			clients[c_id].do_send(&pac);
		}
		else {
			// 실패
			LSC_JOIN_FAIL_PACKET p;
			p.size = sizeof(LSC_JOIN_FAIL_PACKET);
			p.type = LSC_JOIN_FAIL;

			clients[c_id].do_send(&p);
		}
		break;
	}
	case SS_CONNECT_SERVER: {
		cout << "쌍방 연결 확인" << endl;

		SS_CONNECT_SERVER_PACKET* p = reinterpret_cast<SS_CONNECT_SERVER_PACKET*>(packet);
		clients[c_id]._sl.lock();
		if (clients[c_id]._s_state == ST_FREE) {
			clients[c_id]._sl.unlock();
			break;
		}
		if (clients[c_id]._s_state == ST_INGAME) {
			clients[c_id]._sl.unlock();
			disconnect(c_id);
			break;
		}

		strcpy_s(clients[c_id]._name, p->name);
		clients[c_id]._s_state = ST_INGAME;
		clients[c_id]._sl.unlock();

		SS_DATA_PASS_PACKET pi;
		pi.size = sizeof(SS_DATA_PASS_PACKET);
		pi.type = SS_DATA_PASS;
		strcpy_s(pi.name, "lobby");

		clients[c_id].do_send(&pi);

		break;
	}
	}
}

enum EVENT_TYPE { EV_MATCH };
mutex timer_l;

using namespace chrono;

struct TIMER_EVENT {
	int object_id;
	EVENT_TYPE ev;
	chrono::system_clock::time_point act_time;
	int target_id;

	constexpr bool operator < (const TIMER_EVENT& _Left) const
	{
		return (act_time > _Left.act_time);
	}

};

priority_queue<TIMER_EVENT> timer_queue;

void add_timer(int obj_id, int act_time, EVENT_TYPE e_type, int target_id)
{
	TIMER_EVENT ev;
	ev.act_time = system_clock::now() + milliseconds(act_time);
	ev.object_id = obj_id;
	ev.ev = e_type;
	ev.target_id = target_id;

	timer_l.lock();
	timer_queue.push(ev);
	timer_l.unlock();
}

void do_timer()
{
	while (true)
	{
		while (true)
		{
			timer_l.lock();

			if (timer_queue.empty() == true)
			{
				timer_l.unlock();
				break;
			}

			if (timer_queue.top().act_time > chrono::system_clock::now())
			{
				timer_l.unlock();
				break;
			}

			TIMER_EVENT ev = timer_queue.top();
			timer_queue.pop();
			timer_l.unlock();

			switch (ev.ev)
			{
			case EV_MATCH:
			{
				auto ex_over = new OVER_EXP;
				ex_over->_comp_type = OP_UPDATE;
				ex_over->target_id = ev.object_id;
				PostQueuedCompletionStatus(g_h_iocp, 1, ev.target_id, &ex_over->_over);
				add_timer(ev.object_id, 100, ev.ev, ev.target_id);
				break;
			}
			default:
				break;
			}
		}
	}
}

void do_worker()
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(g_h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		int client_id = static_cast<int>(key);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			SOCKET c_socket = reinterpret_cast<SOCKET>(ex_over->_wsabuf.buf);
			int client_id = get_new_client_id();
			if (client_id != -1) {
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._state = 0;
				clients[client_id]._socket = c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, client_id, 0);
				clients[client_id].do_recv();
				c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else cout << "Max user exceeded.\n";

			::ZeroMemory(&ex_over->_over, sizeof(ex_over->_over));
			ex_over->_wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, c_socket, ex_over->_send_buf, 0, addr_size + 16, addr_size + 16, 0, &ex_over->_over);
			break;
		}
		case OP_RECV: {
			if (0 == num_bytes) disconnect(client_id);
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			if (0 == num_bytes) disconnect(client_id);
			delete ex_over;
			break;
		case OP_UPDATE:
			if (test_match_list.size() >= 4) {
				for (int i = 0; i < 4; ++i) {
					LSC_CONGAME_PACKET p;
					p.connect = true;
					p.size = sizeof(LSC_CONGAME_PACKET);
					p.type = LSC_CONGAME;
					clients[test_match_list.front()].do_send(&p);
					test_match_list.pop_front();
				}
			}
			if (stage1_match_list.size() >= 4) {
				for (int i = 0; i < 4; ++i) {
					LSC_CONGAME_PACKET p;
					p.connect = true;
					p.size = sizeof(LSC_CONGAME_PACKET);
					p.type = LSC_CONGAME;
					clients[stage1_match_list.front()].do_send(&p);
					stage1_match_list.pop_front();
				}
			}
			if (stage2_match_list.size() >= 4) {
				for (int i = 0; i < 4; ++i) {
					LSC_CONGAME_PACKET p;
					p.connect = true;
					p.size = sizeof(LSC_CONGAME_PACKET);
					p.type = LSC_CONGAME;
					clients[stage2_match_list.front()].do_send(&p);
					stage2_match_list.pop_front();
				}
			}
			if (stage3_match_list.size() >= 4) {
				for (int i = 0; i < 4; ++i) {
					LSC_CONGAME_PACKET p;
					p.connect = true;
					p.size = sizeof(LSC_CONGAME_PACKET);
					p.type = LSC_CONGAME;
					clients[stage3_match_list.front()].do_send(&p);
					stage3_match_list.pop_front();
				}
			}
			break;
		}
	}
}

void show_error() {
	printf("error\n");
}

SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt = 0;
SQLRETURN retcode;
SQLWCHAR szuser_id[DATA_LEN], szuser_passward[DATA_LEN];
SQLINTEGER duser_level;
SQLLEN cbID = 0, cbPASSWARD = 0, cbLEVEL = 0;

void DB_init() {
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"Algeater", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "db 로딩 완료" << endl;					
				}
			}
		}
	}

}

void DB_basic_load() {
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT user_id, user_passward, user_level FROM user_datas ORDER BY 1", SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		cout << "select OK" << endl;
		// Bind columns 1, 2, and 3  
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szuser_id, DATA_LEN, &cbID);
		retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, szuser_passward, DATA_LEN, &cbPASSWARD);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &duser_level, 100, &cbLEVEL);
		// Fetch and print each row of data. On an error, display a message and exit.  
		for (int i = 0; ; i++) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				show_error();
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				//replace wprintf with printf
				//%S with %ls
				//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
				//but variadic argument 2 has type 'SQLWCHAR *'
				//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
				wprintf(L"%d: %ls %ls %d\n", i + 1, szuser_id, szuser_passward, duser_level);

				wchar_t* t = reinterpret_cast<wchar_t*>(szuser_id);
				int t_len = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
				char* ptr;
				ptr = new char[t_len];
				WideCharToMultiByte(CP_ACP, 0, t, -1, ptr, t_len, 0, 0);
				strcpy(user_datas[i].user_id, ptr);
				delete ptr;

				t = reinterpret_cast<wchar_t*>(szuser_passward);
				t_len = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
				ptr = new char[t_len];
				WideCharToMultiByte(CP_ACP, 0, t, -1, ptr, t_len, 0, 0);
				strcpy(user_datas[i].user_passward, ptr);
				delete ptr;

				user_datas[i].user_level = duser_level;

				int len = strlen(user_datas[i].user_id);
				int index = 0;
				for (int j = 0; j < len; j++) {
					if (!std::isspace(user_datas[i].user_id[j])) {
						user_datas[i].user_id[index++] = user_datas[i].user_id[j];
					}
				}
				user_datas[i].user_id[index] = '\0';

				len = strlen(user_datas[i].user_passward);
				index = 0;
				for (int j = 0; j < len; j++) {
					if (!std::isspace(user_datas[i].user_passward[j])) {
						user_datas[i].user_passward[index++] = user_datas[i].user_passward[j];
					}
				}
				user_datas[i].user_passward[index] = '\0';

				cout << "데이터 로드 성공" << endl;
			}
			else
				break;
		}
	}

	// Process data  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCancel(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}
}

void DB_end() {
	SQLDisconnect(hdbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

SQLWCHAR* CharToSQLWCHAR(const char* str) {
	int len = strlen(str);
	int bufferSize = MultiByteToWideChar(CP_ACP, 0, str, len, nullptr, 0);
	SQLWCHAR* sqlWStr = new SQLWCHAR[bufferSize + 1];
	MultiByteToWideChar(CP_ACP, 0, str, len, sqlWStr, bufferSize);
	sqlWStr[bufferSize] = L'\0'; // NULL 문자 추가
	return sqlWStr;
}

void PrintODBCError(SQLHANDLE handle, SQLSMALLINT handleType) {
	SQLWCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLWCHAR messageText[256];
	SQLSMALLINT textLength;

	SQLRETURN ret;
	SQLSMALLINT i = 1;

	while ((ret = SQLGetDiagRec(handleType, handle, i, sqlState, &nativeError,
		messageText, sizeof(messageText), &textLength)) != SQL_NO_DATA) {
		std::wcout << "ODBC Error: " << sqlState << ", Native Error: " << nativeError << ", Message: " << messageText << std::endl;
		i++;
	}
}

int add_user(const char* id, const char* pw) {

	int is_already = 0;
	
	char tmp[100];

	sprintf(tmp, "SELECT user_id FROM user_datas WHERE user_id = '%s'", id);
	
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, CharToSQLWCHAR(tmp), SQL_NTS);
	
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szuser_id, DATA_LEN, &cbID);

		for (int i = 0; ; i++) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				show_error();
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{				
				wprintf(L"%ls is already", szuser_id);
				is_already++;
			}
			else {
				break;
			}
		}
	}else if(retcode == SQL_NO_DATA) {
		cout << "670 :: 값이 없대? " << endl;
	}
	else if (retcode == SQL_ERROR) {
		cout << "673 :: 썸띵 프라블럼? " << endl;
		if (retcode == SQL_ERROR) {
			PrintODBCError(hstmt, SQL_HANDLE_STMT);
		}
	}

	// Process data  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCancel(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}

	if (is_already != 0) {
		return 0;
	}

	sprintf(tmp, "INSERT INTO user_datas VALUES ('%s', '%s', 1)", id,pw);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, CharToSQLWCHAR(tmp), SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		std::cout << "695 :: 정상 입력 " << std::endl;
	}
	else if (retcode == SQL_ERROR) {
		std::cout << "698 :: 썸띵 프라블럼? " << std::endl;
		PrintODBCError(hstmt, SQL_HANDLE_STMT);
	}

	sprintf(tmp, "SELECT user_id FROM user_datas WHERE user_id = '%s'", id);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, CharToSQLWCHAR(tmp), SQL_NTS);


	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szuser_id, DATA_LEN, &cbID);

		for (int i = 0; ; i++) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				show_error();
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				wprintf(L"%ls add complite", szuser_id);
				is_already++;
			}
			else {
				break;
			}
		}
	}
	else if (retcode == SQL_NO_DATA) {
		cout << "732 ::값이 없대? " << endl;
	}
	else if (retcode == SQL_ERROR) {
		cout << "735 :: 썸띵 프라블럼? " << endl;
		if (retcode == SQL_ERROR) {
			PrintODBCError(hstmt, SQL_HANDLE_STMT);
		}
	}

	// Process data  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLCancel(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}

	if (is_already == 0) {
		return 0;
	}

	return 1;
}

void Data_read() {
	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"Algeater", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					cout << "db 로딩 완료" << endl;
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT user_id, user_passward, user_level FROM user_datas ORDER BY 1", SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						cout << "select OK" << endl;
						// Bind columns 1, 2, and 3  
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szuser_id, DATA_LEN, &cbID);
						retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, szuser_passward, DATA_LEN, &cbPASSWARD);
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &duser_level, 100, &cbLEVEL);
						// Fetch and print each row of data. On an error, display a message and exit.  
						for (int i = 0; ; i++) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
								show_error();
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								//replace wprintf with printf
								//%S with %ls
								//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
								//but variadic argument 2 has type 'SQLWCHAR *'
								//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
								wprintf(L"%d: %ls %ls %d\n", i + 1, szuser_id, szuser_passward, duser_level);

								wchar_t* t = reinterpret_cast<wchar_t*>(szuser_id);
								int t_len = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
								char* ptr;
								ptr = new char[t_len];
								WideCharToMultiByte(CP_ACP, 0, t, -1, ptr, t_len, 0, 0);
								strcpy(user_datas[i].user_id, ptr);
								delete ptr;

								t = reinterpret_cast<wchar_t*>(szuser_passward);
								t_len = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
								ptr = new char[t_len];
								WideCharToMultiByte(CP_ACP, 0, t, -1, ptr, t_len, 0, 0);
								strcpy(user_datas[i].user_passward, ptr);
								delete ptr;

								user_datas[i].user_level = duser_level;

								int len = strlen(user_datas[i].user_id);
								int index = 0;
								for (int j = 0; j < len; j++) {
									if (!std::isspace(user_datas[i].user_id[j])) {
										user_datas[i].user_id[index++] = user_datas[i].user_id[j];
									}
								}
								user_datas[i].user_id[index] = '\0';

								len = strlen(user_datas[i].user_passward);
								index = 0;
								for (int j = 0; j < len; j++) {
									if (!std::isspace(user_datas[i].user_passward[j])) {
										user_datas[i].user_passward[index++] = user_datas[i].user_passward[j];
									}
								}
								user_datas[i].user_passward[index] = '\0';

								cout << "데이터 로드 성공" << endl;
							}			
							else		
								break;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

void Set_user_data()
{
	for (int i = 0; i < MAX_USER; i++) {
		user_datas[i].user_id[0] = { 0 };
		user_datas[i].user_passward[0] = { 0 };
		user_datas[i].user_level = 0;
	}
}

int main()
{
	Set_user_data();
	DB_init();
	DB_basic_load();
	//Data_read();
	char cmp[20] = "admin";
	strcpy(user_datas[MAX_USER - 1].user_id, cmp);
	strcpy(user_datas[MAX_USER - 1].user_passward, cmp);
	user_datas[MAX_USER - 1].user_level = 88;

	cout << "데이터 로딩 완료" << endl;
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(LOBBY_SERVER_PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	g_h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), g_h_iocp, 9999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	a_over._wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
	AcceptEx(g_s_socket, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);


	test_match_list.empty();
	stage1_match_list.empty();
	stage2_match_list.empty();
	stage3_match_list.empty();
	add_timer(0, 1000, EV_MATCH, 0);

	// 게임 서버와 커넥트

	ss_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(GAME_SERVER_PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(GAME_SERVER_IP);

	int client_id;

	if (connect(ss_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cout << "커넥트 실패" << endl;
	}
	else {
		cout << "커넥트 성공" << endl;
		OVER_EXP ss_over;
		ss_over._comp_type = OP_ACCEPT;
		ss_over._wsabuf.buf = reinterpret_cast<CHAR*>(ss_socket);

		SOCKET c_socket = reinterpret_cast<SOCKET>(ss_over._wsabuf.buf);
		client_id = get_new_client_id();
		if (client_id != -1) {
			clients[client_id]._id = client_id;
			clients[client_id]._name[0] = 0;
			clients[client_id]._prev_remain = 0;
			clients[client_id]._socket = c_socket;
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_h_iocp, client_id, 0);
			clients[client_id].do_recv();
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		}
		else cout << "Max user exceeded.\n";

		::ZeroMemory(&ss_over._over, sizeof(ss_over._over));
		ss_over._wsabuf.buf = reinterpret_cast<CHAR*>(c_socket);
		int addr_size = sizeof(SOCKADDR_IN);
		AcceptEx(g_s_socket, c_socket, ss_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &ss_over._over);


		SS_CONNECT_SERVER_PACKET p;
		p.size = sizeof(SS_CONNECT_SERVER_PACKET);
		p.type = SS_CONNECT_SERVER;
		strcpy_s(p.name, "lobby");

		clients[client_id].do_send(&p);
	}


	vector <thread> worker_threads;
	for (int i = 0; i < 6; ++i)
		worker_threads.emplace_back(do_worker);

	thread timer_thread{ do_timer };
	timer_thread.join();

	for (auto& th : worker_threads)
		th.join();

	DB_end();
	closesocket(g_s_socket);
	WSACleanup();
}