#include "Timer.h"

void Timer::InitTimer(WindowInfo& windowInfo)
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
	_hwnd = windowInfo.hwnd;
}


void Timer::TimerUpdate()
{
	unsigned _int64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime;

	if (_frameTime >= 1.f)
	{
		_fps = static_cast<unsigned _int32>(_frameCount / _frameTime);

		_frameTime = 0.f;
		_frameCount = 0;
	}
}

void Timer::ShowFps()
{
	UINT fps = _fps;

	WCHAR text[100] = L"";
	if (isGS == 0)
	{
		::wsprintf(text, L"Performance_of_GeometryShader_in_Particles GS : Off   FPS : %d", fps);
	}
	else if (isGS == 1)
	{
		::wsprintf(text, L"Performance_of_GeometryShader_in_Particles GS : On   FPS : %d", fps);
	}
	

	::SetWindowText(_hwnd, text);
}