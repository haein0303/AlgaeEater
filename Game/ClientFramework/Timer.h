#pragma once

#include "Util.h"

class Timer
{
public:
	float	_deltaTime = 0.f;
	unsigned _int32	_fps = 0;

	void InitTimer();
	
	void TimerUpdate();

	void fixed_update_tic();

	void ShowFps(WindowInfo windowInfo);

private:
	unsigned _int64	_frequency = 0;
	unsigned _int64	_prevCount = 0;
	

	unsigned _int32 _lockFps = 60;

	unsigned _int32	_frameCount = 0;
	float	_frameTime = 0.f;
public:
	unsigned int _counter = 0;

};