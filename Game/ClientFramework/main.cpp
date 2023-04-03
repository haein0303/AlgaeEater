#include "Client.h"
#include "lobby_client.h"

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	
	MSG msg = { 0 };

	cout << "WinMain INIT CALL" << endl;
	LOBBY_CLIENT lobby_client;
	lobby_client.init(hInst, nCmdShow);
	lobby_client.connect_server(LOBBY_PORT_NUM);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			
		}

	}
	cout << "EXIT lobby Msg Queue INIT CALL" << endl;

	Client client;
	//클라이언트 초기화
	client.Init(hInst, nCmdShow);
	//매 프레임마다 업데이트
	cout << "create client" << endl;

	thread logical_thread{ &Client::Logic,&client };
	thread render_thread1{ &Client::Draw,&client };
	thread render_thread2{ &Client::Draw,&client };

	cout << "create threads" << endl;

	ShowCursor(false);
	msg.message = 0;
	while (msg.message != WM_QUIT)
	{
		if (client.dxEngine.inputPtr->_mouse_chaged) {
			client.dxEngine.inputPtr->_mouse_chaged = false;
			if (client.dxEngine.inputPtr->_mouse_enable) {				
				ShowCursor(true);
			}
			else {				
				ShowCursor(false);
			}
		}

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//client.Logic();
		}
		
	}

	logical_thread.join();
	render_thread1.join(); 
	render_thread2.join();



	return (int)msg.wParam;
	
}

