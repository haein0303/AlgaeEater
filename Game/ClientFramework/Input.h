#pragma once
#include "Util.h"
#include "SFML.h"

class OBJECT;

class Input
{
public:
	vector<int> _states;
	//ȭ��ȸ��(���Ŀ� ĳ���� ��ҷ� �ű� ����)
	POINT angle = { 0,0 };

	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;

	bool _mouse_enable = false;
	bool _mouse_chaged = false;


	bool _show_info = false;

	//���� ������ �ʱ�ȭ
	void Init(WindowInfo windowInfo);

	//Ű �Է�
	void InputKey(shared_ptr<Timer> timerPtr, OBJECT* playerArr, shared_ptr<SFML> networkPtr);
	void inputMouse(OBJECT* playerArr, shared_ptr<SFML> networkPtr);

private:
	POINT m_ptOldCursorPos;
	
};