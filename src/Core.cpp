#include"Core.h"
#include"Debug_Draw.h"

Core::Core():
	m_quit(false),
	m_window(NULL),
	m_renderer(NULL),
	m_counted_frames(0)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

	m_window = SDL_CreateWindow("Ball Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (m_window == NULL)
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	if (m_renderer == NULL)
		std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_scene = new Scene();

	Body* b = new Body(Circle(10), Vector2(10, 20));
	m_scene->AddBody(b);

	Body* b1 = new Body(AABB(50, 50), { 80, 90 });
	m_scene->AddBody(b1);
}

Core::~Core()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}

void Core::Loop()
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
				int mouse_x, mouse_y;
				SDL_GetMouseState(&mouse_x, &mouse_y);

				Body* b = new Body(Circle(10), Vector2(mouse_x, mouse_y));
				m_scene->AddBody(b);
			}
		}

		m_scene->Update(1.f / SCREEN_FPS);
		Render();

		++m_counted_frames;


		if (m_fps_timer.GetTicks() > 1000)
		{
			m_fps_timer.Stop();
			std::cout << m_counted_frames << std::endl;
			m_counted_frames = 0;
		}



		int frames_ticks = m_cap_timer.GetTicks();
		if (frames_ticks < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frames_ticks);
	}
}


void Set_Pixel(SDL_Renderer* renderer, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawPoint(renderer, x, y);
}

void Draw_Circle(SDL_Renderer* renderer, int p_x, int p_y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	double error = (double)-radius;
	double x = (double)radius - 0.5;
	double y = (double)0.5;
	double cx = p_x - 0.5;
	double cy = p_y - 0.5;

	while (x >= y)
	{
		Set_Pixel(renderer, (int)(cx + x), (int)(cy + y), r, g, b, a);
		Set_Pixel(renderer, (int)(cx + y), (int)(cy + x), r, g, b, a);

		if (x != 0)
		{
			Set_Pixel(renderer, (int)(cx - x), (int)(cy + y), r, g, b, a);
			Set_Pixel(renderer, (int)(cx + y), (int)(cy - x), r, g, b, a);
		}

		if (y != 0)
		{
			Set_Pixel(renderer, (int)(cx + x), (int)(cy - y), r, g, b, a);
			Set_Pixel(renderer, (int)(cx - y), (int)(cy + x), r, g, b, a);
		}

		if (x != 0 && y != 0)
		{
			Set_Pixel(renderer, (int)(cx - x), (int)(cy - y), r, g, b, a);
			Set_Pixel(renderer, (int)(cx - y), (int)(cy - x), r, g, b, a);
		}

		error += y;
		++y;
		error += y;

		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}


void Core::Render()
{
	SDL_RenderClear(m_renderer);

	for (auto body : m_scene->GetBodies())
	{
		if (body->m_shape->GetType() == CIRCLE)
		{
			Circle* circle = (Circle*)body->m_shape;
			Draw_Circle(m_renderer, body->GetPosition().x, body->GetPosition().y, circle->m_radius, 255, 255, 255, 255);

		}
		else if (body->m_shape->GetType() == AABB_)
		{
			AABB* aabb = (AABB*)body->m_shape;

			float w = aabb->m_width;
			float h = aabb->m_height;

			SDL_Rect rect = { body->GetPosition().x , body->GetPosition().y , w, h};

			SDL_RenderDrawRect(m_renderer, &rect);
		}
	}

	Debug_Draw::GetInstance().Draw(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(m_renderer);
}