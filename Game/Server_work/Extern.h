#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <unordered_set>
#include <windows.h>
#include <string>
#include <queue>
#include <locale.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <random>

#include "protocol.h"
#include "Over_EXP.h"
#include "Session.h"
#include "Timer.h"
#include "Lua_API.h"
#include "NPC.h"

extern "C" {
#include "include/lua.h"
#include "include/lauxlib.h"
#include "include/lualib.h"
}

#pragma comment (lib, "lua54.lib")
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;
using namespace chrono;

array<SESSION, MAX_USER + NPC_NUM> clients;

array<CUBE, 4> cubes;

HANDLE g_h_iocp;
SOCKET g_s_socket;

lua_State* L;

default_random_engine dre;
uniform_int_distribution<> uid{ 0, 3 };

priority_queue<TIMER_EVENT> timer_queue;
mutex timer_l;