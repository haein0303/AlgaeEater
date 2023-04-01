#include "Util.h"
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

LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			
		}
		else
		{
			
		}
		return 0;
	case WM_QUIT:
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