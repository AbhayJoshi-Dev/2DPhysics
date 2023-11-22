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

	//AABB aabb(700, 25);

	//Body* b = new Body(&aabb, { 400, 500 }, true);
	//m_scene->AddBody(b);



	Polygon poly;

	Vector2 vertices[4] = { Vector2(0.f,0.f), Vector2(700.f,0.f) , Vector2(700.f,50.f) , Vector2(0.f,50.f)};
	

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
			float rad = circle->m_radius;
			Draw_Circle(m_renderer, body->m_position.x, body->m_position.y, rad, 255, 255, 255, 255);


			//Vector2 r1(1.f, 0.f);
			//float c = std::cos(body->m_orientation);
			//float s = std::sin(body->m_orientation);

			//Vector2 r;

			//r.x = r1.x * c - r1.y * s;   //v1.x * v2.x - v1.y * v2.y
			//r.y = r1.x * s + r1.y * c;   //v1.x * v2.y + v1.y * v2.x

			//r = r * rad;

			float x = rad * cos(body->m_orientation);
			float y = rad * sin(body->m_orientation);


			Vector2 r(x, y);
			r = r + body->m_position;
			Debug_Draw::GetInstance().DrawSegment(body->m_position, r);


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
			Vector2* vertices = poly->m_vertices;
				
			Mat22 m(body->m_orientation);

			for (int i = 0; i < count; i++)
			{
				SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
				int i1 = i + 1 < count ? i + 1 : 0;


				Vector2 v1 = body->m_position + m * vertices[i];
				Vector2 v2 = body->m_position + m * vertices[i1];

				SDL_RenderDrawLine(m_renderer, v2.x, v2.y, v1.x, v1.y);
			}
		}
	}

	Debug_Draw::GetInstance().Draw(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(m_renderer);
}