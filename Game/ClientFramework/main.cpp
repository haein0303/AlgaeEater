#include "Client.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	Client client;
	//Ŭ���̾�Ʈ �ʱ�ȭ
	client.Init(hInst, nCmdShow);
	//�� �����Ӹ��� ������Ʈ
	MSG msg = { 0 };


	thread logical_thread{ &Client::Update,&client };
	thread render_thread{ &Client::Draw,&client };



	while (msg.message != WM_QUIT)
	{
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			cout << msg.message << endl;
		}
		else
		{
			
		}
	}

	logical_thread.join();
	render_thread.join();


	client.life_control(false);

	return (int)msg.wParam;
	
}

