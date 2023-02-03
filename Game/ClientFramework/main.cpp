#include "Client.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	//cout << "WinMain INIT CALL" << endl;
	Client client;
	//Ŭ���̾�Ʈ �ʱ�ȭ
	client.Init(hInst, nCmdShow);
	//�� �����Ӹ��� ������Ʈ
	MSG msg = { 0 };

	thread logical_thread{ &Client::Logic,&client };
	thread render_thread1{ &Client::Draw,&client };
	thread render_thread2{ &Client::Draw,&client };


	while (msg.message != WM_QUIT)
	{
		
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

