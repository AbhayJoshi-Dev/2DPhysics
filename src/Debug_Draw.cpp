#include"Debug_Draw.h"
#include"Core.h"

Debug_Draw::Debug_Draw()
{

}

Debug_Draw& Debug_Draw::GetInstance()
{
	static Debug_Draw* instance = new Debug_Draw;

	return *instance;
}

void Debug_Draw::DrawSegment(Vector2 start, Vector2 end)
{
	m_lines.emplace_back(start, end);
}

void Debug_Draw::DrawPoint(Vector2 point)
{
	m_points.emplace_back(point.x, point.y);
}

void Debug_Draw::Draw(SDL_Renderer* renderer)
{
	for (int i = 0; i < m_lines.size(); i++)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(renderer, m_lines[i].m_start.x, m_lines[i].m_start.y, m_lines[i].m_end.x, m_lines[i].m_end.y);
	}

	for (int j = 0; j < m_points.size(); j++)
		SDL_RenderDrawPoint(renderer, m_points[j].x, m_points[j].y);

	m_lines.clear();
	m_points.clear();
}