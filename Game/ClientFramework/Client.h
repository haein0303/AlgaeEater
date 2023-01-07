#pragma once
#include "DxEngine.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
bool isActive = true;
atomic<bool> g_isLive = true;

class Client
{
public:
	DxEngine dxEngine; //DX엔진
	WindowInfo windowInfo; //화면 관련 정보 객체

	

	void Init(HINSTANCE hInst, int nCmdShow)
	{
		//윈도우 객체 초기화
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
		WndClass.lpszClassName = _T("DXPractice");
		RegisterClass(&WndClass);
		windowInfo.hwnd = CreateWindow(_T("DXPractice"), _T("DXPractice"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowInfo.ClientWidth, windowInfo.ClientHeight, NULL, NULL, hInst, NULL);
		ShowWindow(windowInfo.hwnd, nCmdShow);
		UpdateWindow(windowInfo.hwnd);
		
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);

		//엔진 초기화
		dxEngine.Init(windowInfo);

		//오브젝트 데이터 생성
		vector<Vertex> cubeVertexVec;
		vector<UINT> cubeIndexVec;
		dxEngine.fbxLoaderPtr->LoadFbxData(cubeVertexVec, cubeIndexVec, "../Resources/Cube.fbx");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(cubeVertexVec, dxEngine.devicePtr, 0);
		dxEngine.indexBufferPtr->CreateIndexBuffer(cubeIndexVec, dxEngine.devicePtr, 0);
		vector<Vertex> playerVertexVec;
		vector<UINT> playerIndexVec;
		dxEngine.fbxLoaderPtr->LoadFbxData(playerVertexVec, playerIndexVec, "../Resources/AnimeCharacter.fbx");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(playerVertexVec, dxEngine.devicePtr, 1);
		dxEngine.indexBufferPtr->CreateIndexBuffer(playerIndexVec, dxEngine.devicePtr, 1);
		vector<Vertex> npcVertexVec;
		vector<UINT> npcIndexVec;
		dxEngine.fbxLoaderPtr->LoadFbxData(npcVertexVec, npcIndexVec, "../Resources/Dragon.fbx");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(npcVertexVec, dxEngine.devicePtr, 2);
		dxEngine.indexBufferPtr->CreateIndexBuffer(npcIndexVec, dxEngine.devicePtr, 2);

		dxEngine.psoPtr->CreateInputLayoutAndPSOAndShader(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\AnimeCharcter.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 0);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\bricks.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 1);
		dxEngine.texturePtr->CreateSRV(dxEngine.devicePtr);

		dxEngine.cmdQueuePtr->WaitSync();
	}

	void Update()
	{
		while (g_isLive) {
			dxEngine.Update(windowInfo, isActive);
		}
		return;
	}

	void Draw() {
		while (g_isLive) {
			dxEngine.Draw();
		}
		return;
	}

	void life_control(bool input) {
		cout << g_isLive << endl;
		g_isLive = input;
	}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			isActive = false;
		}
		else
		{
			isActive = true;
		}
		return 0;
	case WM_DESTROY:
		g_isLive = false;
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		FreeConsole();
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}