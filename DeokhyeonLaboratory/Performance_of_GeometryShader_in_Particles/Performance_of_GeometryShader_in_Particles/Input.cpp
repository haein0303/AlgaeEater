#include "Timer.h"
#include "Input.h"

void Input::Init()
{
	_states.resize(255);
}

void Input::InputKey(shared_ptr<Timer> timer)
{
	HWND hwnd = GetActiveWindow();

	for (UINT key = 0; key < 255; key++)
	{
		if (GetAsyncKeyState(key) & 0x8000)
		{
			if (_states[key] == 1 || _states[key] == 2)
				_states[key] = 1;
			else
				_states[key] = 2;
		}
		else
		{
			int& state = _states[key];

			if (_states[key] == 1 || _states[key] == 2)
				_states[key] = 3;
			else
				_states[key] = 0;
		}
	}

	if (_states['1'] == 2)
	{
		if(timer->isGS == 0)
			timer->isGS = 1;
		else if (timer->isGS == 1)
			timer->isGS = 0;
	}
}