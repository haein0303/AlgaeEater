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
		WndClass.lpszClassName = _T("Performance_of_GeometryShader_in_Particles");
		RegisterClass(&WndClass);
		windowInfo.hwnd = CreateWindow(_T("Performance_of_GeometryShader_in_Particles"), _T("Performance_of_GeometryShader_in_Particles"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowInfo.ClientWidth, windowInfo.ClientHeight, NULL, NULL, hInst, NULL);
		ShowWindow(windowInfo.hwnd, nCmdShow);
		UpdateWindow(windowInfo.hwnd);
		
		//���� �ʱ�ȭ
		dxEngine.Init(windowInfo);

		//������Ʈ ������ ����
		vector<GSPoint> vec(1);
		vec[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vec[0].size = XMFLOAT2(0.5f, 0.5f);
		vector<UINT> indexVec;
		indexVec.push_back(0);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(vec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(indexVec, dxEngine.devicePtr);
		dxEngine.psoPtr->CreateInputLayoutAndPSOAndShader(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.texturePtr->CreateTexture(L"..\\Performance_of_GeometryShader_in_Particles\\resources\\star.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr);
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
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}