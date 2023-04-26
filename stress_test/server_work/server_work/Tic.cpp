#include "Tic.h"

#include <Windows.h>

void Tic::InitTimer()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
}

void Tic::TimerUpdate()
{
	unsigned _int64 currentCount;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

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

void Tic::fixed_update_tic()
{
	unsigned _int64 currentCount;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	while ((1.f / (float)_lockFps) >= (currentCount - _prevCount) / static_cast<float>(_frequency)) {
		_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));
	}
	//std::cout << "\rD_TIME : " << _deltaTime << ":" << (1.f / (float)_lockFps);
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
