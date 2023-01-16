#pragma once
#include "Util.h"
#include "SFML.h"

class Input
{
public:
	vector<int> _states;
	//ȭ��ȸ��(���Ŀ� ĳ���� ��ҷ� �ű� ����)
	POINT angle = { 0,0 };

	//���� ������ �ʱ�ȭ
	void Init(WindowInfo windowInfo);

	//Ű �Է�
	void InputKey(shared_ptr<Timer> timerPtr, Obj* playerArr, shared_ptr<SFML> networkPtr);
	void inputMouse(Obj* playerArr, shared_ptr<SFML> networkPtr);

private:
	POINT m_ptOldCursorPos;
	
};