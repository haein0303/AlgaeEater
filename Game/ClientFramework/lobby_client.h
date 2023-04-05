#pragma once
class LOBBY_CLIENT
{
public:
	shared_ptr<SFML> Lobby_network = make_shared<SFML>();
	HWND _hwnd;
	int LobbyWidth = 1280;
	int LobbyHeight = 720;

	int _is_connected = 0;
	wchar_t _client_msg[200];
	wchar_t _state_msg[4][100];
	int _ready_state = 3;
	


	HWND init(HINSTANCE hInst, int nCmdShow);

	
	int connect_server(int port);

	
	void draw_text(const wchar_t* input);

	
};

LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);