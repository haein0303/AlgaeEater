#include "Timer.h"
#include "AnimationObject.h"
#include "Input.h"
#include "OBJECT.h"

void Input::Init(WindowInfo windowInfo)
{
	_states.resize(255);
	m_ptOldCursorPos.x = windowInfo.ClientWidth / 2 + 100;
	m_ptOldCursorPos.y = windowInfo.ClientHeight / 2 + 100;

	//::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	//ShowCursor(false);
}

bool send_toggle = false;
int counter_toggle = 0;
void Input::InputKey(shared_ptr<Timer> timerPtr, array<OBJECT, PLAYERMAX>& playerArr, shared_ptr<SFML> networkPtr, vector<BoundingBox>& bounding_boxes)
{
		HWND hwnd = GetActiveWindow();
		bool key_toggle = false;

		for (UINT key = 0; key < 255; key++)
		{
			//key 정보
			//
			if (GetAsyncKeyState(key) & 0x8000)
			{
				if (_states[key] == 1 || _states[key] == 2) {

					_states[key] = 1;
				}
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
		if (_states['W'] == 1)
		{
			key_toggle = true;
		}
		if (_states['S'] == 1)
		{
			key_toggle = true;
		}
		if (_states['A'] == 1)
		{
			key_toggle = true;
		}
		if (_states['D'] == 1)
		{
			key_toggle = true;
		}
		if (_states['W'] == 2)
		{
			w = true;
		}
		if (_states['W'] == 3)
		{
			w = false;
		}
		if (_states['S'] == 2)
		{
			s = true;

		}
		if (_states['S'] == 3)
		{
			s = false;

		}
		if (_states['A'] == 2)
		{
			a = true;

		}
		if (_states['A'] == 3)
		{
			a = false;

		}
		if (_states['D'] == 2)
		{
			d = true;

		}
		if (_states['D'] == 3)
		{
			d = false;

		}
		if (_states['1'] == 2)
		{
			CS_CONSOLE_PACKET p;
			p.size = sizeof(p);
			p.type = CS_CONSOLE;
			p.console = 1;
			networkPtr->send_packet(&p);
		}
		if (_states[VK_ESCAPE] == 1) {
			cout << "QUIT" << endl;
			ExitProcess(0);
		}
		if (_states[VK_F2] == 2) {
			_show_info = !_show_info;
		}
		if (_states[VK_F3] == 2) {
			_god_mod_on = !_god_mod_on;
			CS_GOD_MOD_PACKET p;
			p.size = sizeof(p);
			p.type = CS_GOD_MOD;
			networkPtr->send_packet(&p);
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

		if (playerArr[networkPtr->myClientId]._animation_state != AnimationOrder::Death) {
			if (_states[VK_RBUTTON] == 2) {
				playerArr[networkPtr->myClientId]._animation_state = AnimationOrder::Skill;
			}
			else if (_states[VK_LBUTTON] == 2) {
				playerArr[networkPtr->myClientId]._animation_state = AnimationOrder::Attack;
				playerArr[networkPtr->myClientId]._combo_count++;
				cout << "combo : " << playerArr[networkPtr->myClientId]._combo_count << endl;
			}
			else if (!(w == false && a == false && s == false && d == false)
				&& playerArr[networkPtr->myClientId]._animation_state != AnimationOrder::Attack
				&& playerArr[networkPtr->myClientId]._animation_state != AnimationOrder::Skill) {

				float pos_x0 = playerArr[networkPtr->myClientId]._transform.x;
				float pos_z0 = playerArr[networkPtr->myClientId]._transform.z;

				if (w != s && a == d)
				{
					if (w == true)
					{
						playerArr[networkPtr->myClientId]._transform.x += move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f);
						playerArr[networkPtr->myClientId]._transform.z += move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 90.f;
					}
					else if (s == true)
					{
						playerArr[networkPtr->myClientId]._transform.x -= move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f);
						playerArr[networkPtr->myClientId]._transform.z -= move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 270.f;
					}
				}
				else if (w == s && a != d)
				{
					if (a == true)
					{
						playerArr[networkPtr->myClientId]._transform.x -= move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._transform.z -= move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x;
					}
					else if (d == true)
					{
						playerArr[networkPtr->myClientId]._transform.x += move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._transform.z += move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 180.f;
					}
				}
				else if (w != s && a != d)
				{
					if (w == true && a == true && s == false && d == false)
					{
						playerArr[networkPtr->myClientId]._transform.x += move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f + XM_PI / 4.0f);
						playerArr[networkPtr->myClientId]._transform.z += move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f + XM_PI / 4.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 45.f;
					}
					else if (w == true && d == true && s == false && a == false)
					{
						playerArr[networkPtr->myClientId]._transform.x += move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f + XM_PI / 4.0f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._transform.z += move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f + XM_PI / 4.0f - XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 135.f;
					}
					else if (s == true && a == true && w == false && d == false)
					{
						playerArr[networkPtr->myClientId]._transform.x -= move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 4.0f);
						playerArr[networkPtr->myClientId]._transform.z -= move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 4.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 315.f;
					}
					else if (s == true && d == true && w == false && a == false)
					{
						playerArr[networkPtr->myClientId]._transform.x -= move_speed * timerPtr->_deltaTime * cosf(angle.x * XM_PI / 180.f - XM_PI / 4.0f + XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._transform.z -= move_speed * timerPtr->_deltaTime * sinf(angle.x * XM_PI / 180.f - XM_PI / 4.0f + XM_PI / 2.0f);
						playerArr[networkPtr->myClientId]._degree = -angle.x + 225.f;
					}
				}
				playerArr[networkPtr->myClientId]._animation_state = AnimationOrder::Walk;

				playerArr[networkPtr->myClientId]._bounding_box.Center.x = playerArr[networkPtr->myClientId]._transform.x;
				playerArr[networkPtr->myClientId]._bounding_box.Center.z = playerArr[networkPtr->myClientId]._transform.z;

				for (const BoundingBox& bounding_box : bounding_boxes)
				{
					if (playerArr[networkPtr->myClientId]._bounding_box.Intersects(bounding_box))
					{
						playerArr[networkPtr->myClientId]._transform.x = pos_x0;
						playerArr[networkPtr->myClientId]._transform.z = pos_z0;

						playerArr[networkPtr->myClientId]._bounding_box.Center.x = pos_x0;
						playerArr[networkPtr->myClientId]._bounding_box.Center.z = pos_z0;
					}
				}

				for (int i = 0; i < playerArr.size(); ++i)
				{
					if (networkPtr->myClientId != i && playerArr[networkPtr->myClientId]._bounding_box.Intersects(playerArr[i]._bounding_box))
					{
						//playerArr[networkPtr->myClientId]._transform.x = pos_x0;
						//playerArr[networkPtr->myClientId]._transform.z = pos_z0;

						//playerArr[networkPtr->myClientId]._bounding_box.Center.x = pos_x0;
						//playerArr[networkPtr->myClientId]._bounding_box.Center.z = pos_z0;
					}
				}
				
			}
			else if (playerArr[networkPtr->myClientId]._animation_state != AnimationOrder::Attack && playerArr[networkPtr->myClientId]._animation_state != AnimationOrder::Skill) {
				playerArr[networkPtr->myClientId]._animation_state = AnimationOrder::Idle;
			}

			if (playerArr[networkPtr->myClientId]._animation_state0 != playerArr[networkPtr->myClientId]._animation_state) {
				playerArr[networkPtr->myClientId]._animation_time_pos = 0.f;
				playerArr[networkPtr->myClientId]._animation_state0 = playerArr[networkPtr->myClientId]._animation_state;

				// 플레이어의 애니메이션 상태가 바뀌면 패킷 송신
				CS_MOVE_PACKET p;
				p.size = sizeof(p);
				p.type = CS_MOVE;
				p.x = playerArr[networkPtr->myClientId]._transform.x;
				p.y = playerArr[networkPtr->myClientId]._transform.y;
				p.z = playerArr[networkPtr->myClientId]._transform.z;
				p.degree = playerArr[networkPtr->myClientId]._degree;
				p.char_state = playerArr[networkPtr->myClientId]._animation_state;
				//p.client_time = timerPtr->_counter;
				//cout << "SEND : " << timerPtr->_counter << endl;
				networkPtr->send_packet(&p);

				return;
			}

			//cout << key_toggle << endl;
			if (key_toggle) {
				counter_toggle++;
			}
			
			if (counter_toggle > 3) {
				if (send_toggle) {
					send_toggle != send_toggle;
					return;
				}
				CS_MOVE_PACKET p;
				p.size = sizeof(p);
				p.type = CS_MOVE;
				p.x = playerArr[networkPtr->myClientId]._transform.x;
				p.y = playerArr[networkPtr->myClientId]._transform.y;
				p.z = playerArr[networkPtr->myClientId]._transform.z;
				p.degree = playerArr[networkPtr->myClientId]._degree;
				p.char_state = playerArr[networkPtr->myClientId]._animation_state;
				p.client_time = timerPtr->_counter;
				//cout << "SEND : " << timerPtr->_counter << endl;
				networkPtr->send_packet(&p);
				counter_toggle = 0;
			}
		}
	
}

void Input::inputMouse(array<OBJECT, PLAYERMAX>& playerArr, shared_ptr<SFML> networkPtr)
{
	HWND hwnd = GetActiveWindow();	

	bool key_toggle = false;

	//상하각도제한
	float yMin = -20.f;
	float yMax = 40.f;

	float cxDelta = 0.0f, cyDelta = yMin;

	if (_mouse_enable) {
		//cout << "_mouse_enable이 true임으로 클라이언트 내 마우스 조작을 중단합니다" << endl;
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
		p.x = playerArr[networkPtr->myClientId]._transform.x;
		p.y = playerArr[networkPtr->myClientId]._transform.y;
		p.z = playerArr[networkPtr->myClientId]._transform.z;
		p.degree = playerArr[networkPtr->myClientId]._degree;
		networkPtr->send_packet(&p);
	}
	//cout << angle.x << ":" << angle.y << endl;
}


