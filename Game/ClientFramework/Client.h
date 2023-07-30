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
	atomic<bool> _is_single = false;
	int i_now_render_index_for_single = 0;

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
		
		
		
		//엔진 초기화
		dxEngine.Init(windowInfo);
		//
		

		//오브젝트 데이터 생성
		vector<Point> pointVertexvec(1);
		vector<UINT> pointIndexVec;
		pointVertexvec[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pointVertexvec[0].size = XMFLOAT2(0.1f, 0.1f);
		pointIndexVec.push_back(0);
		dxEngine.vertexBufferPtr->CreateVertexBuffer(dxEngine.vertexBufferPtr->_particleVertexBuffer, dxEngine.vertexBufferPtr->_particleVertexBufferView, pointVertexvec, dxEngine.devicePtr);
		dxEngine.indexBufferPtr->CreateIndexBuffer(dxEngine.indexBufferPtr->_particleIndexBuffer, dxEngine.indexBufferPtr->_particleIndexBufferView, pointIndexVec, dxEngine.devicePtr, dxEngine.indexBufferPtr->_particleIndexCount);

		//기본으로 있어야 되는 아가들
		dxEngine.psoPtr->CreateInputLayoutAndPSOAndShader(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.psoPtr->CreateInputLayoutAndGSAndPSO(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		dxEngine.psoPtr->CreateInputLayoutAndPSOAndShaderOfAniamtion(dxEngine.devicePtr, dxEngine.rootSignaturePtr, dxEngine.dsvPtr);
		
		
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\AnimeCharcter.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 0); //AnimeCharcter
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\NPCSpider_DefaultMaterial_AlbedoTransparency.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 1);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_paint_black_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 2);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 3);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 4);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\spider_bare_metal_BaseColor.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 5);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\Star.png", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 6);
		dxEngine.texturePtr->CreateTexture(L"..\\Resources\\Texture\\bricks.dds", dxEngine.devicePtr, dxEngine.cmdQueuePtr, 7);
		
		dxEngine.texturePtr->CreateSRVs(dxEngine.devicePtr);

		dxEngine.cmdQueuePtr->WaitSync();
	}

	void in_scene(int data, int char_sel) {
		dxEngine.Scene_num = data;
		dxEngine.playerArr[0]._character_num = char_sel;
		cout << "in_Scene : " << char_sel << endl;
	}

	//fixed_update
	void Logic()
	{
		cout << "try server connect" << endl;
		if (-1 == dxEngine.networkPtr->ConnectServer(GAME_PORT_NUM,dxEngine.Scene_num, dxEngine.playerArr[0]._character_num)) {
			cout << "SERVER CONNECT FAIL" << endl;
			while (1);
		}
		cout << "complite server connect" << endl;

		cout << "Loading Model data" << endl;
		dxEngine.late_Init(windowInfo); 
		cout << "Complite Model data Loading" << endl;
		
		cout << "LOGIC CALL" << endl;
		while (g_isLive) {
			dxEngine.logicTimerPtr->fixed_update_tic();

			dxEngine.FixedUpdate(windowInfo, isActive);
		}		
	}

	void Draw() {
		
		cout << "DRAW CALL" << endl;
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
			::WaitForSingleObject(dxEngine._renderEvent, INFINITE);

			dxEngine.timerPtr->TimerUpdate();
			
			dxEngine.timerPtr->ShowFps(windowInfo);

			dxEngine.Update(windowInfo, isActive);
			
			dxEngine.Draw_multi(windowInfo, i_now_render_index);
		}
	}
	void single_init() {
		cout << "try server connect" << endl;
		if (-1 == dxEngine.networkPtr->ConnectServer(GAME_PORT_NUM, dxEngine.Scene_num, dxEngine.playerArr[0]._character_num)) {
			cout << "SERVER CONNECT FAIL" << endl;
			//while (1);
		}
		cout << "complite server connect" << endl;

		cout << "Loading Model data" << endl;
		dxEngine.late_Init(windowInfo);
		cout << "Complite Model data Loading" << endl;

	}
	void single_work() {
		
		dxEngine.logicTimerPtr->TimerUpdate();
		dxEngine.FixedUpdate(windowInfo, isActive);

		::WaitForSingleObject(dxEngine._renderEvent, INFINITE);

		dxEngine.timerPtr->TimerUpdate();


		dxEngine.timerPtr->ShowFps(windowInfo);

		dxEngine.Update(windowInfo, isActive);

		dxEngine.Draw_multi(windowInfo, i_now_render_index_for_single);
		if (i_now_render_index_for_single) {
			i_now_render_index_for_single = 0;
		}
		else {
			i_now_render_index_for_single = 1;
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
			//ShowCursor(true);
			//ReleaseCapture();			
			isActive = false;
		}
		else
		{
			//ShowCursor(false);
			//SetCapture(hwnd);
			isActive = true;
		}
		return 0;
	case WM_DESTROY:
		g_isLive = false;
		PostQuitMessage(0);
		break;
	case WM_QUIT:
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