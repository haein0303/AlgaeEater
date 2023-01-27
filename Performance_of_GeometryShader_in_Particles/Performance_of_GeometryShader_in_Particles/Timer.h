#pragma once
#include "Util.h"
class Timer
{
public:
	unsigned _int64	_frequency = 0;
	unsigned _int64	_prevCount = 0;
	float	_deltaTime = 0.f;

	unsigned _int32	_frameCount = 0;
	float	_frameTime = 0.f;
	unsigned _int32	_fps = 0;

	HWND _hwnd;

	int isGS = 0;

	void InitTimer(WindowInfo& windowInfo);
	void TimerUpdate();
	void ShowFps();
};