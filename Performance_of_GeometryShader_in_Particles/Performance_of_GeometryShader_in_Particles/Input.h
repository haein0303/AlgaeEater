#pragma once
#include "Util.h"
class Input
{
public:
	vector<int> _states;

	//���� ������ �ʱ�ȭ
	void Init();

	//Ű �Է�
	void InputKey(shared_ptr<Timer> timer);
};

