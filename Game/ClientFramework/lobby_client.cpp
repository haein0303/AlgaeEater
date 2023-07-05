
#include <atlimage.h>

#include "Util.h"
#include "SFML.h"
#include "lobby_client.h"



extern LOBBY_CLIENT lobby_client;
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

enum SCENE_STATE {
	LOADING,
	FAIL,
	LOG_IN,
	ACOUNT,
	READY
};

int g_scene_state = 0;

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

	//루프면은 로딩은 다시 할 필요는 없으니깐
	if (g_scene_state == SCENE_STATE::READY) {
		g_scene_state = SCENE_STATE::READY;
	}
	else {
		g_scene_state = SCENE_STATE::LOADING;
	}
	

	return hwnd;
}

int LOBBY_CLIENT::connect_server(int port)
{
	if (_is_connected) {
		cout << "SERVER CONNECTED" << endl;
		g_scene_state = SCENE_STATE::LOG_IN;
		return 0;
	}
	cout << "TRY CONNECT SERVER" << endl;
	if (-1 == Lobby_network->ConnectServer(port)) {
		cout << "SERVER CONNECTED FAIL" << endl;
		//g_scene_state = SCENE_STATE::LOG_IN;
		g_scene_state = SCENE_STATE::FAIL;
		return -1;
	}

	/*LCS_LOGIN_PACKET p_LOGIN;
	p_LOGIN.size = sizeof(p_LOGIN);
	p_LOGIN.type = LCS_LOGIN;
	strcpy(p_LOGIN.id, "asdasd");
	strcpy(p_LOGIN.passward, "asdasd");
	Lobby_network->send_packet(&p_LOGIN);*/
	//draw_text(L"SERVER CONNECTED");
	g_scene_state = SCENE_STATE::LOG_IN;
	return 0;
}

void LOBBY_CLIENT::draw_text(const wchar_t* input)
{
	wsprintf(_client_msg, input);
	InvalidateRect(_hwnd, NULL, false);
	UpdateWindow(_hwnd);
}

CImage BG;
CImage basic_BG;
CImage Icon[4];
CImage i_stage_info[4];
CImage i_logo;
CImage i_logo_b;
HFONT hFont;
HFONT title_font;

CImage i_login_bg;
CImage i_join_bg;

string c_id;
string c_pw;

int login_select = 0;



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
	RECT center_rc = { 0,200,1280,720 };
	RECT logo_rc = { 540,140,740,340 };

	RECT login_button_rc = { 460,450,600,500 };
	RECT join_button_rc = { 660,450,810,500 };
	RECT join2_button_rc = { 550,450,700,500 };

	RECT login_id_rc = { 480, 260,800,300 };
	RECT login_pw_rc = { 480, 370,800,400 };


	RECT login_bg_rc = { 250,300,780,476 };
	switch (iMsg)
	{
	case WM_CREATE: {
		SetTimer(hwnd, 1, 200, (TIMERPROC)TimerProc);
		BG.Load(L"..\\Resources\\Lobby\\title_back.png");
		basic_BG.Load(L"..\\Resources\\UserInterface\\title_back.png");
		i_logo.Load(L"..\\Resources\\UserInterface\\logo.png");
		i_logo_b.Load(L"..\\Resources\\UserInterface\\test.png");

		i_login_bg.Load(L"..\\Resources\\Lobby\\title_back_login.png");
		i_join_bg.Load(L"..\\Resources\\Lobby\\title_join.png");
		

		for (int i = 1; i <= 4; ++i) {
			wchar_t loading[40];
			wsprintf(loading, L"..\\Resources\\Lobby\\button_%d.png", i);
			Icon[i - 1].Load(loading);
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

		title_font = CreateFont(
			50, // Height of the font
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


		CreateCaret(hwnd, (HBITMAP)NULL, 2, 40);
		ShowCaret(hwnd);
	}
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			
		}
		else
		{
			
		}
		return 0;
	case WM_PAINT: {
		//기본작업
		{
			hdc = BeginPaint(hwnd, &ps);
			memDC = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, 1280, 720);
			hBrush = CreateSolidBrush(RGB(255, 255, 255));

			SelectObject(memDC, (HBITMAP)hBitmap);

			SelectObject(memDC, hBrush);

			Rectangle(memDC, 0, 0, 1280, 720);

		}
		switch (g_scene_state) {
		case SCENE_STATE::LOADING: //서버 접속 및 로딩 창
			basic_BG.Draw(memDC, 0, 0, 1280, 720);
			i_logo.Draw(memDC, 490, 140, 300, 300);
			SelectObject(memDC, title_font);
			SetBkMode(memDC, TRANSPARENT);
			SetTextColor(memDC, RGB(255, 255, 255));
			DrawText(memDC, L"접속 중", -1, &center_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			break;
		case SCENE_STATE::FAIL: // 실패 시

			basic_BG.Draw(memDC, 0, 0, 1280, 720);
			i_logo_b.Draw(memDC, 490, 140, 300, 300);
			SelectObject(memDC, title_font);
			SetBkMode(memDC, TRANSPARENT);
			SetTextColor(memDC, RGB(255, 255, 255));
			
			DrawText(memDC, L"서버와 연결할 수 없습니다. 연결을 확인해주세요.", -1, &center_rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			break;
		case SCENE_STATE::LOG_IN: {
			i_login_bg.Draw(memDC, 0, 0, 1280, 720);
			wstring idString(c_id.begin(), c_id.end());
			wstring pwString(c_pw.begin(), c_pw.end());
			SelectObject(memDC, title_font);
			SetBkMode(memDC, TRANSPARENT);
			SetTextColor(memDC, RGB(255, 255, 255));
			if (login_select == 1) {
				SetCaretPos(480+ c_id.length()*12, 250);
			}
			if (login_select == 2) {
				SetCaretPos(480 + c_pw.length() * 12, 360);
			}
			DrawText(memDC, const_cast<LPWSTR>(idString.c_str()), -1, &login_id_rc, DT_LEFT | DT_SINGLELINE);
			DrawText(memDC, const_cast<LPWSTR>(pwString.c_str()), -1, &login_pw_rc, DT_LEFT | DT_SINGLELINE);
		}	
		break;
		case SCENE_STATE::ACOUNT: {
			i_join_bg.Draw(memDC, 0, 0, 1280, 720);

			wstring idString(c_id.begin(), c_id.end());
			wstring pwString(c_pw.begin(), c_pw.end());
			SelectObject(memDC, title_font);
			SetBkMode(memDC, TRANSPARENT);
			SetTextColor(memDC, RGB(255, 255, 255));
			if (login_select == 1) {
				SetCaretPos(480 + c_id.length() * 12, 250);
			}
			if (login_select == 2) {
				SetCaretPos(480 + c_pw.length() * 12, 360);
			}
			DrawText(memDC, const_cast<LPWSTR>(idString.c_str()), -1, &login_id_rc, DT_LEFT | DT_SINGLELINE);
			DrawText(memDC, const_cast<LPWSTR>(pwString.c_str()), -1, &login_pw_rc, DT_LEFT | DT_SINGLELINE);

		}
			break;
		case SCENE_STATE::READY:
			basic_BG.Draw(memDC, 0, 0, 1280, 720);

			i_stage_info[lobby_client._scene_select].Draw(memDC, 990, -40, 250, 250);

			break;
		}

		BitBlt(hdc, 0, 0, 1280, 720, memDC, 0, 0, SRCCOPY);


		EndPaint(hwnd, &ps);

	}
		break;
	case WM_QUIT:

		
		
		DeleteObject(hFont);
		::cout << "WM_QUIT" << endl;
		break;
	case WM_DESTROY:
		::cout << "WM_DESTROY" << endl;
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		::cout << "WM_CLOSE" << endl;
		//FreeConsole();
		break;
	case WM_KEYDOWN:
		switch (g_scene_state) {
			case SCENE_STATE::LOG_IN: 
			case SCENE_STATE::ACOUNT: {
				switch (wParam) {
				case VK_BACK:
					if (login_select == 1) {
						if (c_id.length() > 1) {
							c_id.pop_back();
							c_id.resize(c_id.length() - 1);
							//cout << "POP : " << c_id << endl;
						}
					}
					if (login_select == 2) {
						if (c_pw.length() > 1) {
							c_pw.pop_back();
							c_pw.resize(c_pw.length() - 1);
							//cout << "POP : " << c_id << endl;
						}
					}
					UpdateWindow(hwnd);
					break;
				}
			}
			break;
			case SCENE_STATE::READY:{
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
						p_MATCH.stage = lobby_client._scene_select;
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
			}
			break;

		}
		break;
	case WM_CHAR: {
		switch (g_scene_state) {
		case SCENE_STATE::LOG_IN: case SCENE_STATE::ACOUNT:{
			char ch = static_cast<char>(wParam);
			if (login_select == 1) {
				if (c_id.length() < 10) {
					c_id += ch;
				}
				else {
					cout << "열글자 초과" << endl;
				}
			}

			if (login_select == 2) {
				if (c_pw.length() < 10) {
					c_pw += ch;
				}
				else {
					cout << "열글자 초과" << endl;
				}
			}
			
		}break;
		}
	}
	InvalidateRect(hwnd, NULL, false);
	break;
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		cout << "X : " << x << " Y : " << y << endl;

		if (g_scene_state == SCENE_STATE::LOG_IN) {

			if (onClicK_check(login_button_rc, x, y)) {
				LCS_LOGIN_PACKET p;
				p.size = sizeof(p);
				p.type = LCS_LOGIN;
				strcpy(p.id, c_id.c_str());
				strcpy(p.passward, c_pw.c_str());
				lobby_client.Lobby_network->send_packet(&p);
				cout << "로그인 클릭" << endl;
			}
			if (onClicK_check(join_button_rc, x, y)) {
				c_id.erase();
				c_pw.erase();
				g_scene_state = SCENE_STATE::ACOUNT;
				cout << "조인 클릭" << endl;
				
			}
			
			if(onClicK_check(login_id_rc, x, y)){
				login_select = 1;
				ShowCaret(hwnd);
			}
			else if (onClicK_check(login_pw_rc, x, y)) {
				login_select = 2;
				ShowCaret(hwnd);
			}
			else {
				login_select = 0;
				HideCaret(hwnd);
			}
		}else if (g_scene_state == SCENE_STATE::ACOUNT) {

			if (onClicK_check(join2_button_rc, x, y)) {
				g_scene_state = SCENE_STATE::LOG_IN;
				c_id.erase();
				c_pw.erase();
			}

			if (onClicK_check(login_id_rc, x, y)) {
				login_select = 1;
				ShowCaret(hwnd);
			}
			else if (onClicK_check(login_pw_rc, x, y)) {
				login_select = 2;
				ShowCaret(hwnd);
			}
			else {
				login_select = 0;
				HideCaret(hwnd);
			}
		}
		cout << "g_scene_state : " << g_scene_state << endl;
		InvalidateRect(hwnd, NULL, false);

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

bool onClicK_check(const RECT& rc, const int& x, const int& y) {
	return (x >= rc.left && x <= rc.right && y >= rc.top && y <= rc.bottom);
}