#include "Client.h"
#include "lobby_client.h"
#if _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
LOBBY_CLIENT lobby_client;
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	
	{
		

		cout << "WinMain INIT CALL" << endl;

		lobby_client.init(hInst, nCmdShow);
		lobby_client._is_connected = lobby_client.connect_server(LOBBY_PORT_NUM);
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		cout << "EXIT lobby Msg Queue INIT CALL" << endl;

		Client client;
		client.in_scene(lobby_client._scene_select, lobby_client._char_select);
		//Ŭ���̾�Ʈ �ʱ�ȭ
		client.Init(hInst, nCmdShow);
		//�� �����Ӹ��� ������Ʈ
		cout << "create client" << endl;


		//thread logical_thread{ &Client::Logic,&client };

		//thread render_thread1{ &Client::Draw,&client };
		//thread render_thread2{ &Client::Draw,&client };

		

		cout << "create threads" << endl;
		//�̱�
		client.single_init();
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
				client.single_work();
			}

		}

		//logical_thread.join();
		//render_thread1.join();
		//render_thread2.join();
		
		
	}
	

	

	return (int)msg.wParam;
	
}

