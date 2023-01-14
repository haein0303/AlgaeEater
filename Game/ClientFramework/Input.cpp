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


	if (_states['W'] == 1)
	{
		playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(XM_PI / 2.0f);
		playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(XM_PI / 2.0f);
		key_toggle = true;
	}
	if (_states['S'] == 1)
	{
		playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(XM_PI / 2.0f);
		playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(XM_PI / 2.0f);
		key_toggle = true;
	}
	if (_states['A'] == 1)
	{
		playerArr[networkPtr->myClientId].transform.x -= 5.0f * timerPtr->_deltaTime * cosf(0.0f);
		playerArr[networkPtr->myClientId].transform.z -= 5.0f * timerPtr->_deltaTime * sinf(0.0f);
		key_toggle = true;
	}
	if (_states['D'] == 1)
	{
		playerArr[networkPtr->myClientId].transform.x += 5.0f * timerPtr->_deltaTime * cosf(0.0f);
		playerArr[networkPtr->myClientId].transform.z += 5.0f * timerPtr->_deltaTime * sinf(0.0f);
		key_toggle = true;
	}
	
	if (_states['1'] == 1)
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
		//p.degree = playerArr[myClientId].rotate.y;
		p.x = playerArr[networkPtr->myClientId].transform.x;
		p.y = playerArr[networkPtr->myClientId].transform.y;
		p.z = playerArr[networkPtr->myClientId].transform.z;
		networkPtr->send_packet(&p);
	}
	
}

void Input::inputMouse(POINT &angle, Obj* playerArr, shared_ptr<SFML> networkPtr)
{

	HWND hwnd = GetActiveWindow();	


	bool key_toggle = false;

	//상하각도제한
	float yMin = 20.f;
	float yMax = 90.f;

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
		angle.x += cxDelta;
		if (angle.x > 360.f) angle.x -= 360.f;
		if (angle.x < 0.f) angle.x += 360.f;
		playerArr[networkPtr->myClientId].degree = angle.x;
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
		//p.degree = playerArr[myClientId].rotate.y;
		p.x = playerArr[networkPtr->myClientId].transform.x;
		p.y = playerArr[networkPtr->myClientId].transform.y;
		p.z = playerArr[networkPtr->myClientId].transform.z;
		networkPtr->send_packet(&p);
	}
	//cout << angle.x << ":" << angle.y << endl;
}


