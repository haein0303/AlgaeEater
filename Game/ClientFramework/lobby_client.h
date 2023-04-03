#pragma once
class LOBBY_CLIENT
{
public:
	shared_ptr<SFML> Lobby_network = make_shared<SFML>();
	HWND _hwnd;
	int LobbyWidth = 1280;
	int LobbyHeight = 720;

	HWND init(HINSTANCE hInst, int nCmdShow);

	int _is_connected = 0;
	
	int connect_server(int port);
};

LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);