#pragma once
#include "DxEngine.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

class Client
{
public:
	DxEngine dxEngine; //DX����
	WindowInfo windowInfo; //ȭ�� ���� ���� ��ü

	void Init(HINSTANCE hInst, int nCmdShow)
	{
		//������ ��ü �ʱ�ȭ
		WNDCLASS WndClass;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = WndProc;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = hInst;
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.lpszMenuName = NULL;
		WndClass.lpszClassName = _T("SkinningAnimation");
		RegisterClass(&WndClass);
		windowInfo.hwnd = CreateWindow(_T("SkinningAnimation"), _T("SkinningAnimation"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowInfo.ClientWidth, windowInfo.ClientHeight, NULL, NULL, hInst, NULL);
		ShowWindow(windowInfo.hwnd, nCmdShow);
		UpdateWindow(windowInfo.hwnd);
		
		freopen("CONOUT$", "wt", stdout);

		//���� �ʱ�ȭ
		dxEngine.Init(windowInfo);

		//�簢�� ������ ����
		std::vector<SkinnedVertex> vertices;
		std::vector<UINT> indices;
		dxEngine.LoadObj(vertices, indices);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(vertices, dxEngine.devicePtr, 0);
		dxEngine.indexBufferPtr->CreateIndexBuffer(indices, dxEngine.devicePtr, 0);
		dxEngine.psoPtr->CreateInputLayoutAndShaderAndPSO(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.texturePtr->CreateTexture(L"..\\SkinningAnimation\\resources\\bricks.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr);
		dxEngine.texturePtr->CreateSRV(dxEngine.devicePtr);

		dxEngine.cmdQueuePtr->WaitSync();
	}

	int Update()
	{
		//�� �����Ӹ��� ������Ʈ
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				//�� �����Ӹ��� �׸���
				dxEngine.Draw();
			}
		}

		return (int)msg.wParam;
	}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CLOSE:
		FreeConsole();
		break;
	case WM_CREATE:
		AllocConsole();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}