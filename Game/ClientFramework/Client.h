#pragma once
#include "DxEngine.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
bool isActive = true;
atomic<bool> g_isLive = true;

class Client
{
public:
	DxEngine dxEngine; //DX����
	WindowInfo windowInfo; //ȭ�� ���� ���� ��ü

	void Init(HINSTANCE hInst, int nCmdShow)
	{

		//cout << "Client INIT CALL" << endl;

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
		WndClass.lpszClassName = _T("DXPractice");
		RegisterClass(&WndClass);
		windowInfo.hwnd = CreateWindow(_T("DXPractice"), _T("DXPractice"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, windowInfo.ClientWidth, windowInfo.ClientHeight, NULL, NULL, hInst, NULL);
		ShowWindow(windowInfo.hwnd, nCmdShow);
		UpdateWindow(windowInfo.hwnd);
		
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);


		//���� �ʱ�ȭ
		dxEngine.Init(windowInfo);

		//������Ʈ ������ ����
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

	//fixed_update
	void Logic()
	{
		while (g_isLive) {
			dxEngine.logicTimerPtr->fixed_update_tic();

			dxEngine.FixedUpdate(windowInfo, isActive);
		}		
	}

	void Draw() {

		//cout << "DRAW CALL" << endl;

		while (g_isLive) {
			//cout << "UPDATE";
			float fTimeElapsed = 0;
			float fLockFPS =10.f;
			dxEngine.timerPtr->TimerUpdate();
			if (!isActive) { //��Ƽ�� ���°� �ƴҶ�				
				while (fTimeElapsed < (1.f / fLockFPS)) {
					dxEngine.timerPtr->TimerUpdate();
					fTimeElapsed += dxEngine.timerPtr->_deltaTime;
				}	
				dxEngine.timerPtr->_fps = static_cast<int>(1.f / fTimeElapsed);
				cout << "\rDRAW BACK GROUND";
			}
			else {
				cout << "\rACTIVE";
			}
			dxEngine.timerPtr->ShowFps(windowInfo);

			dxEngine.Update(windowInfo, isActive);
			dxEngine.Draw(windowInfo);
		}
	}

	void life_control(bool input) {
		//cout << g_isLive << endl;
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
			ShowCursor(true);
			ReleaseCapture();			
			isActive = false;
		}
		else
		{
			ShowCursor(false);
			SetCapture(hwnd);
			isActive = true;
		}
		return 0;
	case WM_DESTROY: case WM_QUIT:
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