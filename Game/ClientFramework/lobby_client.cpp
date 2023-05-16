
#include <atlimage.h>

#include "Util.h"
#include "SFML.h"
#include "lobby_client.h"



extern LOBBY_CLIENT lobby_client;
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

HWND LOBBY_CLIENT::init(HINSTANCE hInst, int nCmdShow)
{	
	

	HWND hwnd;
	//윈도우 객체 초기화
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = Lobby_WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInst;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("AlgaeEater_lobby");
	RegisterClass(&WndClass);
	_hwnd = hwnd = CreateWindow(_T("AlgaeEater_lobby"), _T("AlgaeEater_lobby"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, LobbyWidth, LobbyHeight, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);	

	wsprintf(_state_msg[0], L"READY");
	wsprintf(_state_msg[1], L"WAIT");
	wsprintf(_state_msg[2], L"SERVER NOT CONNECT");
	wsprintf(_state_msg[3], L"LOADING");


	return hwnd;
}

int LOBBY_CLIENT::connect_server(int port)
{
	if (_is_connected) {
		draw_text(L"SERVER CONNECTED");
		_ready_state = 1;
		return 0;
	}
	draw_text(L"TRY CONNECT SERVER");
	if (-1 == Lobby_network->ConnectServer(port)) {
		_ready_state = 2;
		draw_text(L"SERVER CONNECTED FAIL");
		return -1;
	}

	LCS_LOGIN_PACKET p_LOGIN;
	p_LOGIN.size = sizeof(p_LOGIN);
	p_LOGIN.type = LCS_LOGIN;
	Lobby_network->send_packet(&p_LOGIN);
	draw_text(L"SERVER CONNECTED");
	_ready_state = 1;
	return 0;
}

void LOBBY_CLIENT::draw_text(const wchar_t* input)
{
	wsprintf(_client_msg, input);
	InvalidateRect(_hwnd, NULL, false);
	UpdateWindow(_hwnd);
}

CImage BG;
CImage Icon[4];
CImage i_stage_info[4];
HFONT hFont;


LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	//화면 출력용
	HDC hdc;
	//더블 버퍼링용
	HDC memDC;
	//비트맵 이미지 출력용
	//memDC와 호환된다
	HDC hMemDC;

	static HBITMAP hBitmap;
	PAINTSTRUCT ps;


	static HBRUSH hBrush;
	static HBRUSH B_INGAME_BG;

	RECT Game_info_rc = { 5, 5, 1000, 35 };
	RECT Game_state_rc = { 0, 650, 1100, 680 };
	RECT Game_ready_rc = { 0, 620, 1280, 680 };

	switch (iMsg)
	{
	case WM_CREATE:
		SetTimer(hwnd, 1, 200, (TIMERPROC)TimerProc);
		BG.Load(L"..\\Resources\\Lobby\\title_back.png");
		for (int i = 1; i <= 4; ++i) {
			wchar_t loading[40];
			wsprintf(loading,L"..\\Resources\\Lobby\\button_%d.png", i);
			Icon[i-1].Load(loading);
		}
		for (int i = 0; i < 4; ++i) {
			wchar_t loading[40];
			wsprintf(loading, L"..\\Resources\\Lobby\\stage%d.png", i);
			i_stage_info[i].Load(loading);
		}

		AddFontResource(L"..\\Resources\\Lobby\\BusanBada.ttf");

		hFont = CreateFont(
			30, // Height of the font
			0, // Width of the font (0 = default)
			0, // Angle of escapement (0 = default)
			0, // Orientation angle (0 = default)
			FW_NORMAL, // Font weight (normal)
			FALSE, // Italic
			FALSE, // Underline
			FALSE, // Strikeout
			DEFAULT_CHARSET, // Character set
			OUT_DEFAULT_PRECIS, // Output precision
			CLIP_DEFAULT_PRECIS, // Clipping precision
			DEFAULT_QUALITY, // Output quality
			DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
			L"BusanBada" // Font name (must match the actual font name in the file)
		);
		wsprintf(lobby_client._client_msg, L"LOADING");

		
		

		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			
		}
		else
		{
			
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		memDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, 1280, 720);
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		
		
		
		
		SelectObject(memDC, (HBITMAP)hBitmap);

		SelectObject(memDC, hBrush);

		Rectangle(memDC, 0, 0, 1280, 720);


		BG.Draw(memDC, 0, 0, 1280, 720);
		i_stage_info[lobby_client._scene_select].Draw(memDC, 990, -40, 250, 250);
		

		SelectObject(memDC, hFont);
		SetBkMode(memDC, TRANSPARENT);
		DrawText(memDC, lobby_client._client_msg, -1, &Game_info_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		Icon[lobby_client._ready_state].Draw(memDC, 1280 - 176, 720 - 190, 126, 140);
		DrawText(memDC, lobby_client._state_msg[lobby_client._ready_state], -1, &Game_state_rc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

		DrawText(memDC, L"Press Space for Ready", -1, &Game_ready_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		BitBlt(hdc, 0, 0, 1280, 720, memDC, 0, 0, SRCCOPY);

		
		EndPaint(hwnd, &ps);
		break;
	case WM_QUIT:

		
		
		DeleteObject(hFont);
		cout << "WM_QUIT" << endl;
		break;
	case WM_DESTROY:
		cout << "WM_DESTROY" << endl;
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		cout << "WM_CLOSE" << endl;
		//FreeConsole();
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_SPACE:
			if (lobby_client._ready_state == 0) {
				LCS_OUT_PACKET p_MATCH_OUT;
				p_MATCH_OUT.size = sizeof(p_MATCH_OUT);
				p_MATCH_OUT.type = LCS_OUT;
				lobby_client.Lobby_network->send_packet(&p_MATCH_OUT);
				lobby_client.draw_text(L"SEND MATCH CANCLE PACKET");
				lobby_client._ready_state = 1;
			}
			else {
				LCS_MATCH_PACKET p_MATCH;
				p_MATCH.size = sizeof(p_MATCH);
				p_MATCH.type = LCS_MATCH;
				p_MATCH.stage = 0;
				lobby_client.Lobby_network->send_packet(&p_MATCH);
				lobby_client.draw_text(L"SEND MATCH PACKET");
				lobby_client._ready_state = 0;
			}
			
			break;
		case VK_LEFT:
			lobby_client._scene_select = (lobby_client._scene_select + 3) % 4;
			break;
		case VK_RIGHT:
			lobby_client._scene_select = (lobby_client._scene_select + 1) % 4;
			break;
		
		}		
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	switch (idEvent) {
	case 1:
		lobby_client.Lobby_network->ReceiveServer(lobby_client._ready_state);
		if (lobby_client._ready_state == -1) {
			PostQuitMessage(0);
			DestroyWindow(lobby_client._hwnd);
			lobby_client._ready_state = 3;
		}
		break;
	
	}
	InvalidateRect(hWnd, NULL, false);
}