#include<imgui.h>

#include"Draw.h"


//Draw& Draw::GetInstance()
//{
	//static Draw* instance = new Draw();

//	return nullptr;
//}

struct RenderLines
{
	RenderLines()
		:m_count(0)
	{}

	void Draw(SDL_Renderer* renderer)
	{
		for (int i = 0; i < m_count - 1;)
		{
			SDL_SetRenderDrawColor(renderer, m_colors[i].r, m_colors[i].g, m_colors[i].b, m_colors[i].a);
			SDL_RenderDrawLine(renderer, m_vertices[i].x, m_vertices[i].y, m_vertices[i + 1].x, m_vertices[i + 1].y);
			i += 2;
		}
		m_count = 0;
		m_vertices.clear();
		m_colors.clear();
	}

	void Vertex(const Vector2& v, const Color& c)
	{
		m_vertices.emplace_back(v);
		m_colors.emplace_back(c);
		m_count++;
	}

	std::vector<Vector2> m_vertices;
	std::vector<Color> m_colors;
	int m_count;
};

Draw::Draw()
{
	m_lines = new RenderLines();
}

void Draw::DrawCircle(const Vector2& center, float radius, const Vector2& axis, const Color& color)
{
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * 3.14159265359f / k_segments;

	Mat22 r(k_increment);
	Vector2 r1(1.0f, 0.0f);
	Vector2 v1 = center + radius * r1;

	for (int i = 0; i < k_segments; ++i)
	{
		//roataing vector r1 with matrix r
		Vector2 r2 = r * r1;
		Vector2 v2 = center + radius * r2;

		m_lines->Vertex(v1, color);
		m_lines->Vertex(v2, color);

		r1 = r2;
		v1 = v2;
	}

	m_lines->Vertex(center, color);
	Vector2 ax = center + radius * axis;
	m_lines->Vertex(ax, color);
}

void Draw::DrawSegment(const Vector2& v1, const Vector2& v2, const Color& color)
{
	m_lines->Vertex(v1, color);
	m_lines->Vertex(v2, color);
}

void Draw::DrawPolygon(const Vector2* vertices, int vertexCount, const Color& color)
{
	Vector2 v1 = vertices[vertexCount - 1];
	for (int i = 0; i < vertexCount; i++)
	{
		Vector2 v2 = vertices[i];
		m_lines->Vertex(v1, color);
		m_lines->Vertex(v2, color);
		v1 = v2;
	}
}

void Draw::DrawString(const Vector2& position, const char* string)
{
	ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
	ImGui::SetCursorPos(ImVec2(position.x, position.y));
	ImGui::TextColored(ImColor(230, 153, 153, 255), string);
	ImGui::End();
}

void Draw::_Draw(SDL_Renderer* renderer)
{
	m_lines->Draw(renderer);
}