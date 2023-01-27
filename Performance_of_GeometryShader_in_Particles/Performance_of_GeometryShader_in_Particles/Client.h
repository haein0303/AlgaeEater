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

		//�簢�� ������ ����
		vector<Vertex> ractangleVec(4);
		ractangleVec[0].pos = XMFLOAT3(-0.5f, 0.5f, 0.0f);
		ractangleVec[0].color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
		ractangleVec[0].uv = XMFLOAT2(0.f, 0.f);
		ractangleVec[1].pos = XMFLOAT3(0.5f, 0.5f, 0.0f);
		ractangleVec[1].color = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
		ractangleVec[1].uv = XMFLOAT2(1.f, 0.f);
		ractangleVec[2].pos = XMFLOAT3(0.5f, -0.5f, 0.0f);
		ractangleVec[2].color = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
		ractangleVec[2].uv = XMFLOAT2(1.f, 1.f);
		ractangleVec[3].pos = XMFLOAT3(-0.5f, -0.5f, 0.0f);
		ractangleVec[3].color = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
		ractangleVec[3].uv = XMFLOAT2(0.f, 1.f);
		vector<UINT> ractangleIndexVec;
		ractangleIndexVec.push_back(0);
		ractangleIndexVec.push_back(1);
		ractangleIndexVec.push_back(2);
		ractangleIndexVec.push_back(0);
		ractangleIndexVec.push_back(2);
		ractangleIndexVec.push_back(3);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(ractangleVec, dxEngine.devicePtr, 0);
		dxEngine.indexBufferPtr->CreateIndexBuffer(ractangleIndexVec, dxEngine.devicePtr, 0);

		//�� ������ ����
		vector<GSPoint> vec(1);
		vec[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vec[0].size = XMFLOAT2(1.0f, 1.0f);
		vector<UINT> indexVec;
		indexVec.push_back(0);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(vec, dxEngine.devicePtr, 1);
		dxEngine.indexBufferPtr->CreateIndexBuffer(indexVec, dxEngine.devicePtr, 1);

		dxEngine.psoPtr->CreateInputLayoutAndGSAndPSO(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.psoPtr->CreateInputLayoutAndShaderAndPSO(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.texturePtr->CreateTexture(L"..\\Performance_of_GeometryShader_in_Particles\\resources\\star.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr);
		dxEngine.texturePtr->CreateSRV(dxEngine.devicePtr);
		dxEngine.inputPtr->Init(); //���� ������ �ʱ�ȭ

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