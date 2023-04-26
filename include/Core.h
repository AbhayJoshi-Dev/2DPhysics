#pragma once

#include<iostream>
#include<SDL.h>

#include"Timer.h"
#include"Scene.h"

class Core
{
public:
	Core();
	~Core();

	void Loop();
	void Render();

private:

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;

	const int SCREEN_FPS = 60;
	const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

	bool m_quit;

	SDL_Event m_event;

	Timer m_fps_timer;
	Timer m_cap_timer;

	int m_counted_frames;

	Scene *m_scene;
};