#pragma once

#include<SDL.h>
#include<vector>

#include"MathUtils.h"
#include"DebugDraw.h"

struct RenderLines;

struct LineSegment
{
	Vector2 m_start;
	Vector2 m_end;

	LineSegment(const Vector2& start,const Vector2& end):
		m_start(start), m_end(end)
	{}
};

class Draw : public DebugDraw
{
public:
	Draw();
	//static Draw& GetInstance();
	void DrawCircle(const Vector2& center, float radius, const Vector2& axis, const Color& color) override;
	void DrawSegment(const Vector2& v1, const Vector2& v2, const Color& color) override;
	void DrawPolygon(const Vector2* vertices, int vertexCount, const Color& color) override;
	void DrawAABB(const AABB* aabb, const Color& color) override;
	void _Draw(SDL_Renderer *renderer);

private:
	RenderLines* m_lines;
};