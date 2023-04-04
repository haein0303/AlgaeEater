
#include <atlimage.h>

#include "Util.h"
#include "SFML.h"
#include "lobby_client.h"



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

	return hwnd;
}

int LOBBY_CLIENT::connect_server(int port)
{
	if (_is_connected) {
		return 1;
	}
	Lobby_network->ConnectServer(port);
	return 0;
}

CImage BG;
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

	RECT rc = { 10, 10, 500, 500 };
	const wchar_t* text = L"Hello, world!";

	

	

	switch (iMsg)
	{
	case WM_CREATE:
		BG.Load(L"..\\Resources\\Lobby\\test.png");
		AddFontResource(L"..\\Resources\\Lobby\\BusanBada.ttf");

		hFont = CreateFont(
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

		SelectObject(memDC, hFont);
		SetBkMode(memDC, TRANSPARENT);
		DrawText(memDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

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
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}