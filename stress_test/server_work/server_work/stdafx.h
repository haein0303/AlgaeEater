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

#include "protocol.h"
