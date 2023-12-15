#include"Test.h"
#include<imgui.h>
#include<imgui_impl_sdl2.h>
#include<imgui_impl_sdlrenderer2.h>


Test::Test():
	m_quit(false),
	m_window(NULL),
	m_renderer(NULL)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

	m_window = SDL_CreateWindow("2D Physics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (m_window == NULL)
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	if (m_renderer == NULL)
		std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer2_Init(m_renderer);


	m_scene = new Scene();

	Polygon poly;

	Vector2 vertices[4] = { Vector2(1500.f,0.f), Vector2(0.f,0.f) , Vector2(0.f,50.f) , Vector2(1500.f,50.f) };
	

	poly.Set(vertices, 4);

	Body* b1 = new Body(&poly, Vector2(50.f, 500.f), true);
	m_scene->AddBody(b1);

	m_scene->SetDebugDraw(&m_draw);

	srand(3);

	m_currentTime = (float)SDL_GetTicks64();
}

Test::~Test()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}

void Test::Step()
{
	while (!m_quit)
	{
		float newTime = (float)SDL_GetTicks64();
		float frameTime = newTime - m_currentTime;
		m_currentTime = newTime;


		m_cap_timer.Start();

		while (SDL_PollEvent(&m_event) != 0)
		{
			ImGui_ImplSDL2_ProcessEvent(&m_event);
			if (m_event.type == SDL_QUIT)
				m_quit = true;

			if (m_event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (m_event.button.button == SDL_BUTTON_LEFT)
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					Circle circle(15.f);
					Body* b = new Body(&circle, Vector2((float)mouse_x, (float)mouse_y), false);
					m_scene->AddBody(b);
				}
				else if (m_event.button.button == SDL_BUTTON_RIGHT)
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					Polygon poly;
					int v = Random(50, 100);
					Vector2 vertices[4];
					for (int i = 0; i < 4; i++)
						vertices[i] = { (float)Random(-v, v), (float)Random(-v, v) };

					poly.Set(vertices, 4);
					Body* b1 = new Body(&poly, Vector2((float)mouse_x, (float)mouse_y), false);
					m_scene->AddBody(b1);
				}
			}
			else if (m_event.type == SDL_KEYDOWN)
			{
				if (m_event.key.keysym.sym == SDLK_SPACE)
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);


					Circle circle(15.f);
					Body* b = new Body(&circle, Vector2((float)mouse_x, (float)mouse_y), false);
					m_scene->AddBody(b); 
				}
			}
		}

		UpdateUI(frameTime);

		m_scene->Step(1.f / SCREEN_FPS);

		Render();

		uint32_t frames_ticks = m_cap_timer.GetTicks();
		if (frames_ticks < SCREEN_TICKS_PER_FRAME)
			SDL_Delay(SCREEN_TICKS_PER_FRAME - (float)frames_ticks);
	}
}

void Test::Render()
{
	ImGui::Render();

	SDL_RenderClear(m_renderer);

	m_scene->Draw();
	m_draw._Draw(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

	SDL_RenderPresent(m_renderer);
}

void Test::UpdateUI(float timeStep)
{
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(float(SCREEN_WIDTH), float(SCREEN_HEIGHT)));
	ImGui::SetNextWindowBgAlpha(0.0f);
	//ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
	//ImGui::End();
	char c[20];
	sprintf_s(c, "%0.1f ms", timeStep);
	m_draw.DrawString(Vector2(10.f, 10.f), c);

	sprintf_s(c, "Total bodies: %d", int(m_scene->GetBodies().size()));
	m_draw.DrawString(Vector2(10.f, 30.f), c);

	ImGui::SetNextWindowPos(ImVec2(float(SCREEN_WIDTH) - 150.f, 20.f));
	ImGui::SetNextWindowSize(ImVec2(100.f, float(SCREEN_HEIGHT) - 40.f));

	//ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	//ImGui::End();

}