#pragma once

#include <SDL.h>

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Pause();
	void UnPause();

	uint32_t GetTicks();

	bool IsStarted();
	bool IsPaused();

private:
	uint32_t m_startTicks;
	uint32_t m_pausedTicks;

	bool m_paused;
	bool m_started;
};