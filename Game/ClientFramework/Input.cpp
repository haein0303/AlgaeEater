#include "Timer.h"
#include "Input.h"

void Input::Init(WindowInfo windowInfo)
{
	_states.resize(255);
	m_ptOldCursorPos.x = windowInfo.ClientWidth / 2 + 100;
	m_ptOldCursorPos.y = windowInfo.ClientHeight / 2 + 100;

	::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
}

void Input::InputKey(shared_ptr<Timer> timerPtr, Obj* playerArr, shared_ptr<SFML> networkPtr)
{
	HWND hwnd = GetActiveWindow();
	bool key_toggle = false;

	for (UINT key = 0; key < 255; key++)
	{
		//key 정보
		//
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

	//이동 처리
	if (_states['W'] == 2)
	{
		w = true;
		key_toggle = true;
	}
	if (_states['W'] == 3)
	{
		w = false;
		key_toggle = true;
	}
	if (_states['S'] == 2)
	{
		s = true;
		key_toggle = true;
	}
	if (_states['S'] == 3)
	{
		s = false;
		key_toggle = true;
	}
	if (_states['A'] == 2)
	{
		a = true;
		key_toggle = true;
	}
	if (_states['A'] == 3)
	{
		a = false;
		key_toggle = true;
	}
	if (_states['D'] == 2)
	{
		d = true;
		key_toggle = true;
	}
	if (_states['D'] == 3)
	{
		d = false;
		key_toggle = true;
	}

	//w = 8, a = 4, s = 2, d = 6, wa = 7, wd = 9, sa = 1, sd = 3 , none or full = 5 
	if (w != s && a == d)
	{
		if (w == true)
			keyNum = 8;
		else
			keyNum = 2;
	}
	else if (w == s && a != d)
	{
		if (a == true)
			keyNum = 4;
		else
			keyNum = 6;
	}
	else if (w != s && a != d)
	{
		if (w == true && a == true)
			keyNum = 7;
		else if (w == true && d == true)
			keyNum = 9;
		else if (s == true && a == true)
			keyNum = 1;
		else
			keyNum = 3;
	}
	else
	{
		keyNum = 5;
	}
	
	if (_states['1'] == 2)
	{
		CS_CONSOLE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_CONSOLE;
		p.console = 1;
		networkPtr->send_packet(&p);
	}

	// 키가 눌렸었다면 패킷 송신
	if (key_toggle) {
		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.keyNum = keyNum;
		p.cameraX = angle.x;
		networkPtr->send_packet(&p);
	}
	
}

void Input::inputMouse(Obj* playerArr, shared_ptr<SFML> networkPtr)
{
	HWND hwnd = GetActiveWindow();	

	bool key_toggle = false;

	//상하각도제한
	float yMin = -10.f;
	float yMax = 40.f;

	float cxDelta = 0.0f, cyDelta = yMin;


	if (GetCapture() == hwnd) {
		::SetCursor(NULL);
		POINT ptCursorPos;
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	if (cxDelta != 0.f) {
		angle.x -= cxDelta;
		if (angle.x > 360.f) angle.x = 0.f;
		if (angle.x < 0.f) angle.x = 360.f;
	}

	if (cyDelta != 0.f) {
		angle.y += cyDelta;
		if(angle.y > yMax) angle.y = yMax;
		if(angle.y < yMin) angle.y = yMin;			
	}

	if (key_toggle) {
		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.keyNum = keyNum;
		p.cameraX = angle.x;
		networkPtr->send_packet(&p);
	}
	//cout << angle.x << ":" << angle.y << endl;
}


