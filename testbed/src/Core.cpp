#include"Core.h"
#include"Draw.h"

Core::Core():
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



	srand(3);
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

void Core::Render()
{
	SDL_RenderClear(m_renderer);

	for (auto body : m_scene->GetBodies())
	{
		if (body->m_shape->GetType() == CIRCLE)
		{
			Circle* circle = (Circle*)body->m_shape;
			float radius = circle->m_radius;

			Mat22 rot(body->m_orientation);

			draw.DrawCircle(body->m_position, radius, rot * Vector2(1.f, 0.f), Color(150, 255, 150, 255));
		}
		else if (body->m_shape->GetType() == AABB_)
		{
			AABB* aabb = (AABB*)body->m_shape;

			float w = aabb->m_width;
			float h = aabb->m_height;

			SDL_Rect rect = { body->m_position.x - w / 2, body->m_position.y - h / 2 , w, h};

			SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(m_renderer, &rect);
		}
		else if (body->m_shape->GetType() == POLYGON)
		{
			Polygon* poly = (Polygon*)body->m_shape;
			int count = poly->m_count;
				
			Mat22 rot(body->m_orientation);

			Vector2 vertices[MAX_POLYGON_VERTICES];

			for (int i = 0; i < count; i++)
			{
				vertices[i] = body->m_position + rot * poly->m_vertices[i];
			}

			draw.DrawPolygon(vertices, count, Color(150, 255, 150, 255));

		}
	}

	//Draw::GetInstance()._Draw(m_renderer);
	draw._Draw(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(m_renderer);
}