#include "Client.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	Client client;
	//클라이언트 초기화
	client.Init(hInst, nCmdShow);
	//매 프레임마다 업데이트
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

