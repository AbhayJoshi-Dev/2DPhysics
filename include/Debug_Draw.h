#pragma once

#include<SDL.h>
#include<vector>

#include"MathUtils.h"

struct LineSegment
{
	Vector2 m_start;
	Vector2 m_end;

	LineSegment(const Vector2& start,const Vector2& end):
		m_start(start), m_end(end)
	{}
};

class Debug_Draw
{
public:
	Debug_Draw();
	static Debug_Draw& GetInstance();
	void DrawSegment(Vector2 start, Vector2 end);
	void DrawPoint(Vector2 point);
	void Draw(SDL_Renderer *renderer);

private:
	std::vector<LineSegment> m_lines;
	std::vector<Vector2> m_points;
};