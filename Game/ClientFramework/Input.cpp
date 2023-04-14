#include "Timer.h"
#include "AnimationObject.h"
#include "Input.h"

void Input::Init(WindowInfo windowInfo)
{
	_states.resize(255);
	m_ptOldCursorPos.x = windowInfo.ClientWidth / 2 + 100;
	m_ptOldCursorPos.y = windowInfo.ClientHeight / 2 + 100;

	//::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//ShowCursor(false);
}

void Input::InputKey(shared_ptr<Timer> timerPtr, Obj* playerArr, shared_ptr<SFML> networkPtr)
{
	HWND hwnd = GetActiveWindow();
	bool key_toggle = false;

	for (UINT key = 0; key < 255; key++)
	{
		//key ����
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

	//�̵� ó��
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
	if (_states[VK_ESCAPE] == 1) {
		cout << "QUIT" << endl;
		ExitProcess(0);
	}
	if (_states[VK_TAB] == 2) {
		cout << "TAP" << endl;
		_mouse_enable = !_mouse_enable;
		_mouse_chaged = true;
		if (_mouse_enable) {
			
			ShowCursor(true);
			cout << "ShowCursor(true);" << endl;
		}
		else {
			ShowCursor(false);
			cout << "ShowCursor(false);" << endl;
		}
		
	}
	//w = 8, a = 4, s = 2, d = 6, wa = 7, wd = 9, sa = 1, sd = 3 , none or full = 5 
	if (w != s && a == d)
	{
		if (w == true)
		{
			playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f);
			playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 90.f;
		}
		else if(s == true)
		{
			playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f);
			playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 270.f;
		}
	}
	else if (w == s && a != d)
	{
		if (a == true)
		{
			playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x;
		}
		else if(d == true)
		{
			playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 180.f;
		}
	}
	else if (w != s && a != d)
	{
		if (w == true && a == true && s == false && d == false)
		{
			playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f + XM_PI / 4.0f);
			playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f + XM_PI / 4.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 45.f;
		}
		else if (w == true && d == true && s == false && a == false)
		{
			playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f + XM_PI / 4.0f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f + XM_PI / 4.0f - XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 135.f;
		}
		else if (s == true && a == true && w == false && d == false)
		{
			playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 4.0f);
			playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 4.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 315.f;
		}
		else if (s == true && d == true && w == false && a == false)
		{
			playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 4.0f + XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 4.0f + XM_PI / 2.0f);
			playerArr[networkPtr->myClientId].degree = -angle.x + 225.f;
		}
	}
	
	if (!(w == false && a == false && s == false && d == false)) {
		playerArr[networkPtr->myClientId].animation_state = 1;
	}
	else {
		playerArr[networkPtr->myClientId].animation_state = 0;
	}

	if (_states['1'] == 2)
	{
		CS_CONSOLE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_CONSOLE;
		p.console = 1;
		networkPtr->send_packet(&p);
	}

	// �ִϸ��̼� ���°� �ٲ�� ��Ŷ �۽�
	if (playerArr[networkPtr->myClientId].animation_state0 != playerArr[networkPtr->myClientId].animation_state) {
		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.x = playerArr[networkPtr->myClientId].transform.x;
		p.y = playerArr[networkPtr->myClientId].transform.y;
		p.z = playerArr[networkPtr->myClientId].transform.z;
		p.degree = playerArr[networkPtr->myClientId].degree;
		p.char_state = playerArr[networkPtr->myClientId].animation_state;
		networkPtr->send_packet(&p);
		playerArr[networkPtr->myClientId].animation_state0 = playerArr[networkPtr->myClientId].animation_state;
	}
}

void Input::inputMouse(Obj* playerArr, shared_ptr<SFML> networkPtr)
{
	HWND hwnd = GetActiveWindow();	

	bool key_toggle = false;

	//���ϰ�������
	float yMin = -20.f;
	float yMax = 40.f;

	float cxDelta = 0.0f, cyDelta = yMin;

	if (_mouse_enable) {
		//cout << "_mouse_enable�� true������ Ŭ���̾�Ʈ �� ���콺 ������ �ߴ��մϴ�" << endl;
		return;
	}


	if (GetCapture() == hwnd) {
		::SetCursor(NULL);
		POINT ptCursorPos;
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		//m_ptOldCursorPos = ptCursorPos;
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
		p.x = playerArr[networkPtr->myClientId].transform.x;
		p.y = playerArr[networkPtr->myClientId].transform.y;
		p.z = playerArr[networkPtr->myClientId].transform.z;
		p.degree = playerArr[networkPtr->myClientId].degree;
		networkPtr->send_packet(&p);
	}
	//cout << angle.x << ":" << angle.y << endl;
}


