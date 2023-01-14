#pragma once
#include "Util.h"
#include "SFML.h"

class Input
{
public:
	vector<int> _states;

	//���� ������ �ʱ�ȭ
	void Init(WindowInfo windowInfo);

	//Ű �Է�
	void InputKey(shared_ptr<Timer> timerPtr, Obj* playerArr, shared_ptr<SFML> networkPtr);
	void inputMouse(POINT &angle, Obj* playerArr, shared_ptr<SFML> networkPtr);

private:
	POINT m_ptOldCursorPos;
};