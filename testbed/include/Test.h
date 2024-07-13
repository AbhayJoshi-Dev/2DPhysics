#pragma once

#include<iostream>
#include<SDL.h>

#include"Timer.h"
#include"Draw.h"

class Scene;

class Test
{
public:
	Test();
	~Test();

	void Step();
	void Render();
	void UpdateUI(float timeStep);

private:

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	const int SCREEN_FPS = 60;
	const float SCREEN_TICKS_PER_FRAME = 1000.f / (float)SCREEN_FPS;

	bool m_quit;

	SDL_Event m_event;

	Timer m_cap_timer;

	Scene *m_scene;

	Draw m_draw;
	float m_currentTime;
	const char* m_currentShape;
	bool m_canSpawn = true;
	const char* m_shapes[2];
	bool m_drawAABB;
	bool m_drawTree;

	const char* m_currentApproach;
	const char* m_approachs[2];
};