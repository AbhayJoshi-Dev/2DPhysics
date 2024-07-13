#include<imgui.h>
#include<imgui_impl_sdl2.h>
#include<imgui_impl_sdlrenderer2.h>
#include<implot.h>

#include"Test.h"
#include"Scene.h"

struct ScrollingBuffer {
	int MaxSize;
	int Offset;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000) {
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);
	}
	void AddPoint(float x, float y) {
		if (Data.size() < MaxSize)
			Data.push_back(ImVec2(x, y));
		else {
			Data[Offset] = ImVec2(x, y);
			Offset = (Offset + 1) % MaxSize;
		}
	}
	void Erase() {
		if (Data.size() > 0) {
			Data.shrink(0);
			Offset = 0;
		}
	}
};


Test::Test():
	m_quit(false),
	m_window(NULL),
	m_renderer(NULL)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

	m_window = SDL_CreateWindow("2D Physics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (m_window == NULL)
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	if (m_renderer == NULL)
		std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer2_Init(m_renderer);


	m_scene = new Scene();

	Polygon poly;

	Vector2 vertices[4] = { Vector2(1000.f,0.f), Vector2(0.f,0.f) , Vector2(0.f,25.f) , Vector2(1000.f,25.f) };	

	poly.Set(vertices, 4);

	Body* b1 = new Body(&poly, Vector2(550.f, 675.f), staticBody, m_scene);
	m_scene->AddBody(b1);

	m_scene->SetDebugDraw(&m_draw);

	m_currentTime = (float)SDL_GetTicks64();
	m_shapes[0] = "circle";
	m_shapes[1] = "polygon";
	m_currentShape = m_shapes[0];
	m_drawAABB = false;
	m_drawTree = false;

	m_approachs[0] = "binary tree";
	m_approachs[1] = "brute force (n squared)";
	m_currentApproach = m_approachs[0];
}

Test::~Test()
{	
	delete m_scene;

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImPlot::DestroyContext();
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

			if (m_canSpawn && m_event.type == SDL_KEYDOWN && m_event.key.keysym.sym == SDLK_SPACE)
			{
				//m_canSpawn = false;
				if (m_currentShape == m_shapes[0])
				{
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					Circle circle(15.f);
					Body* b = new Body(&circle, Vector2((float)mouse_x, (float)mouse_y), dynamicBody, m_scene);
					m_scene->AddBody(b);
				}
				else if (m_currentShape == m_shapes[1])
				{
					srand(SDL_GetTicks());
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					Polygon poly;
					int v = Random(50, 100);
					Vector2 vertices[4];
					for (int i = 0; i < 4; i++)
						vertices[i] = { (float)Random(-v, v), (float)Random(-v, v) };

					poly.Set(vertices, 4);
					Body* b1 = new Body(&poly, Vector2((float)mouse_x, (float)mouse_y), dynamicBody, m_scene);
					m_scene->AddBody(b1);
				}
			}
			if (!m_canSpawn && m_event.type == SDL_KEYUP && m_event.key.keysym.sym == SDLK_SPACE)
				m_canSpawn = true;

			if (m_event.type == SDL_KEYDOWN && m_event.key.keysym.sym == SDLK_w)
			{
				int mouse_x, mouse_y;
				SDL_GetMouseState(&mouse_x, &mouse_y);
				Polygon poly;
				Vector2 vertices[4] = { Vector2(20.f, 20.f), Vector2(20.f, -20.f) , Vector2(-20.f, -20.f) , Vector2(-20.f, 20.f) };

				poly.Set(vertices, 4);
				Body* b1 = new Body(&poly, Vector2((float)mouse_x, (float)mouse_y), dynamicBody, m_scene);
				m_scene->AddBody(b1);
			}
		}

		UpdateUI(frameTime);

		m_scene->Step(1.f / SCREEN_FPS);


		//draw aabbs
		if (m_drawAABB || m_drawTree)
		{
			TreeNode* nodes = m_scene->bp.GetTree().GetNodes();
			int nodeCount = m_scene->bp.GetTree().GetNodeCount();

			for (int i = 0; i < nodeCount; i++)
			{
				if(m_drawTree || nodes[i].IsLeaf())
					m_draw.DrawAABB(&nodes[i].aabb, Color(255, 255, 255, 255));
			}
		}

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

	ImGui::SetNextWindowPos(ImVec2(10.f, 20.f));
	ImGui::SetNextWindowSize(ImVec2(400.f, 400.f));


	//test window
	ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	char c[30];
	sprintf_s(c, "frame time: %0.1f ms", timeStep);
	ImGui::Text(c);

	sprintf_s(c, "bodies: %d", int(m_scene->GetBodies().size()));
	ImGui::Text(c);

	sprintf_s(c, "contact manifolds: %d", int(m_scene->GetTotalContacts()));
	ImGui::Text(c);

	sprintf_s(c, "broad phase pairs: %d", int(m_scene->bp.GetPairCount()));
	ImGui::Text(c);

	ImGui::Separator();

	if (ImGui::BeginCombo("spawn( space key )", m_currentShape))
	{
		for (int n = 0; n < IM_ARRAYSIZE(m_shapes); n++)
		{
			bool is_selected = (m_currentShape == m_shapes[n]);
			if (ImGui::Selectable(m_shapes[n], is_selected))
				m_currentShape = m_shapes[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("pair collision approach", m_currentApproach))
	{
		for (int n = 0; n < IM_ARRAYSIZE(m_approachs); n++)
		{
			bool is_selected = (m_currentApproach == m_approachs[n]);
			if (ImGui::Selectable(m_approachs[n], is_selected))
				m_currentApproach = m_approachs[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (m_currentApproach == m_approachs[0])
		m_scene->m_bruteForce = false;
	else
		m_scene->m_bruteForce = true;

	ImGui::Separator();

	ImGui::Checkbox("draw aabbs", &m_drawAABB);

	ImGui::Checkbox("draw tree", &m_drawTree);



	static ScrollingBuffer sdata1, sdata2;
	ImVec2 mouse = ImGui::GetMousePos();
	static float t = 0;
	t += ImGui::GetIO().DeltaTime;
	sdata1.AddPoint(t, timeStep);

	static float history = 20.0f;

	static ImPlotAxisFlags flags = ImPlotAxisFlags_AutoFit;

	if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
		ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
		ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 14, 30);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		//ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("frame time", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
		//ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
		ImPlot::EndPlot();
	}

	if(ImGui::Button("Quit", ImVec2(50.f, 20.f)))
	{
		m_quit = true;
	}

	ImGui::End();

}