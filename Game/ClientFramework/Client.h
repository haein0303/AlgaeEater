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

	atomic<int> _render_thread_num = 0;

	void Init(HINSTANCE hInst, int nCmdShow)
	{

		//cout << "Client INIT CALL" << endl;

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

		cout << "finish settings" << endl;

		//오브젝트 데이터 생성
		vector<Vertex> cubeVertexVec;
		vector<UINT> cubeIndexVec;
		dxEngine.fbxLoaderPtr->LoadObject(cubeVertexVec, cubeIndexVec, "../Resources/Cube.txt");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(dxEngine.vertexBufferPtr->_vertexBuffer, dxEngine.vertexBufferPtr->_vertexBufferView, cubeVertexVec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(dxEngine.indexBufferPtr->_indexBuffer, dxEngine.indexBufferPtr->_indexBufferView, cubeIndexVec, dxEngine.devicePtr, dxEngine.indexBufferPtr->_indexCount);
		cout << "finish loading cube" << endl;
		vector<Vertex> playerVertexVec;
		vector<UINT> playerIndexVec;
		dxEngine.fbxLoaderPtr->LoadObject(playerVertexVec, playerIndexVec, "../Resources/AnimeCharacter.txt");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(dxEngine.vertexBufferPtr->_playerVertexBuffer, dxEngine.vertexBufferPtr->_playerVertexBufferView, playerVertexVec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(dxEngine.indexBufferPtr->_playerIndexBuffer, dxEngine.indexBufferPtr->_playerIndexBufferView, playerIndexVec, dxEngine.devicePtr, dxEngine.indexBufferPtr->_playerIndexCount);
		cout << "finish loading AnimeCharcter" << endl;
		vector<Vertex> npcVertexVec;
		vector<UINT> npcIndexVec;
		dxEngine.fbxLoaderPtr->LoadObject(npcVertexVec, npcIndexVec, "../Resources/mechanical_spider.txt");
		dxEngine.vertexBufferPtr->CreateVertexBuffer(dxEngine.vertexBufferPtr->_npcVertexBuffer, dxEngine.vertexBufferPtr->_npcVertexBufferView, npcVertexVec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(dxEngine.indexBufferPtr->_npcIndexBuffer, dxEngine.indexBufferPtr->_npcIndexBufferView, npcIndexVec, dxEngine.devicePtr, dxEngine.indexBufferPtr->_npcIndexCount);
		cout << "finish loading mechanical_spider" << endl;
		vector<Point> pointVertexvec(1);
		vector<UINT> pointIndexVec;
		pointVertexvec[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pointVertexvec[0].size = XMFLOAT2(0.25f, 0.25f);
		pointIndexVec.push_back(0);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(dxEngine.vertexBufferPtr->_particleVertexBuffer, dxEngine.vertexBufferPtr->_particleVertexBufferView, pointVertexvec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(dxEngine.indexBufferPtr->_particleIndexBuffer, dxEngine.indexBufferPtr->_particleIndexBufferView, pointIndexVec, dxEngine.devicePtr, dxEngine.indexBufferPtr->_particleIndexCount);

		

		dxEngine.psoPtr->CreateInputLayoutAndPSOAndShader(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.psoPtr->CreateInputLayoutAndGSAndPSO(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\AnimeCharcter.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 0);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_paint_yellow_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 1);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 2);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 3);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 4);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 5);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\Star.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 6);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\bricks.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 7);
		
		dxEngine.texturePtr->CreateSRVs(dxEngine.devicePtr);

		dxEngine.cmdQueuePtr->WaitSync();

		cout << "game init finish" << endl;
	}

	//fixed_update
	void Logic()
	{
		while (g_isLive) {
			dxEngine.FixedUpdate(windowInfo, isActive);
			dxEngine.logicTimerPtr->fixed_update_tic();			
		}		
	}

	void Draw() {
		
		//cout << "DRAW CALL" << endl;
		int i_now_render_index;
		if (!_render_thread_num) {
			i_now_render_index = 0;
			_render_thread_num++;
		}
		else {
			i_now_render_index = 1;
			_render_thread_num = 0;
		}

		while (g_isLive) {
			//cout << "UPDATE";
			float fTimeElapsed = 0;
			float fLockFPS = 30.f;
			dxEngine.timerPtr->TimerUpdate();
			if (!isActive) { //액티브 상태가 아닐때				
				while (fTimeElapsed < (1.f / fLockFPS)) {
					dxEngine.timerPtr->TimerUpdate();
					fTimeElapsed += dxEngine.timerPtr->_deltaTime;
				}	
				dxEngine.timerPtr->_fps = static_cast<int>(1.f / fTimeElapsed);
				//cout << "\rDRAW BACK GROUND\n";
			}
			else {
				//cout << "\rACTIVE\n";
			}
			dxEngine.timerPtr->ShowFps(windowInfo);

			dxEngine.Update(windowInfo, isActive);
			dxEngine.Draw_multi(windowInfo, i_now_render_index);
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
		ExitProcess(0);
		break;
	case WM_CLOSE:
		FreeConsole();
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}