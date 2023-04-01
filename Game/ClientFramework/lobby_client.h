#pragma once
class LOBBY_CLIENT
{
public:
	HWND _hwnd;
	int LobbyWidth = 1280;
	int LobbyHeight = 720;

	HWND init(HINSTANCE hInst, int nCmdShow);


};

LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);