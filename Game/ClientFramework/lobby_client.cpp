
#include <atlimage.h>

#include "Util.h"
#include "SFML.h"
#include "lobby_client.h"



HWND LOBBY_CLIENT::init(HINSTANCE hInst, int nCmdShow)
{
	HWND hwnd;
	//������ ��ü �ʱ�ȭ
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
	//���� ����� �غ� �Ǹ� �ּ� Ǯ�� 23.04.01 23:13
	//Lobby_network->ConnectServer(LOBBY_PORT_NUM);

	return hwnd;
}

CImage BG;

LRESULT CALLBACK Lobby_WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	//ȭ�� ��¿�
	HDC hdc;
	//���� ���۸���
	HDC memDC;
	//��Ʈ�� �̹��� ��¿�
	//memDC�� ȣȯ�ȴ�
	HDC hMemDC;

	static HBITMAP hBitmap;
	PAINTSTRUCT ps;


	static HBRUSH hBrush;
	static HBRUSH B_INGAME_BG;


	switch (iMsg)
	{
	case WM_CREATE:
		BG.Load(L"..\\Resources\\Lobby\\test.jpg");
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

		BitBlt(hdc, 0, 0, 1280, 720, memDC, 0, 0, SRCCOPY);


		EndPaint(hwnd, &ps);
		break;
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