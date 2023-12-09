#include"Test.h"
//#include"draw.h"

Test::Test():
	m_quit(false),
	m_window(NULL),
	m_renderer(NULL),
	m_counted_frames(0)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

	m_window = SDL_CreateWindow("Ball Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (m_window == NULL)
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	if (m_renderer == NULL)
		std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_scene = new Scene();

	Polygon poly;

	Vector2 vertices[4] = { Vector2(700.f,0.f), Vector2(0.f,0.f) , Vector2(0.f,50.f) , Vector2(700.f,50.f) };
	

	poly.Set(vertices, 4);

	Body* b1 = new Body(&poly, Vector2(50.f, 500.f), true);
	m_scene->AddBody(b1);

	m_scene->SetDebugDraw(&draw);

	srand(3);
}

Test::~Test()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}

void Test::Step()
{
	while (!m_quit)
	{
		m_cap_timer.Start();

		if(!m_fps_timer.IsStarted())
			m_fps_timer.Start();

		while (SDL_PollEvent(&m_event) != 0)
		{
			if (m_event.type == SDL_QUIT)
				m_quit = true;

			if (m_event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (m_event.button.button == SDL_BUTTON_LEFT)
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);


					Circle circle(15.f);
					Body* b = new Body(&circle, Vector2(mouse_x, mouse_y), false);
					m_scene->AddBody(b);
				}
				else if (m_event.button.button == SDL_BUTTON_RIGHT)
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);

					Polygon poly;

					int v = Random(50, 100);

					Vector2* vertices = new Vector2[4];
					for (int i = 0; i < 4; i++)
						vertices[i] = { (float)Random(-v, v), (float)Random(-v, v) };

					poly.Set(vertices, 4);

					Body* b1 = new Body(&poly, Vector2(mouse_x, mouse_y), false);
					m_scene->AddBody(b1);

					delete [] vertices;
				}
			}
		}

		m_scene->Update(1.f / SCREEN_FPS);
		Render();

		m_counted_frames++;

		if (m_fps_timer.GetTicks() > 1000.f)
		{
			m_fps_timer.Stop();
			std::cout << m_counted_frames << std::endl;
			m_counted_frames = 0;
		}

		uint32_t frames_ticks = m_cap_timer.GetTicks();
		if (frames_ticks < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frames_ticks);
	}
}

void Test::Render()
{
	SDL_RenderClear(m_renderer);

	//Draw::GetInstance()._Draw(m_renderer);
	m_scene->Draw();
	draw._Draw(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(m_renderer);
}