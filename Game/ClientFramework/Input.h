#pragma once
#include "Util.h"
#include "SFML.h"

class OBJECT;

class Input
{
public:
	vector<int> _states;
	//화면회전(이후에 캐릭터 요소로 옮길 예정)
	POINT angle = { 0,0 };

	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;

	bool _mouse_enable = false;
	bool _mouse_chaged = false;


	bool _show_info = false;

	//벡터 사이즈 초기화
	void Init(WindowInfo windowInfo);

	//키 입력
	void InputKey(shared_ptr<Timer> timerPtr, OBJECT* playerArr, shared_ptr<SFML> networkPtr);
	void inputMouse(OBJECT* playerArr, shared_ptr<SFML> networkPtr);

private:
	POINT m_ptOldCursorPos;
	
};