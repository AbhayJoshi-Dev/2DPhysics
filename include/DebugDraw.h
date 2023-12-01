#pragma once

struct Color
{
	Color() {}

	Color(int _r, int _g, int _b, int _a)
		:r(_r), g(_g), b(_b), a(_a)
	{}

	int r, g, b, a;
};

class DebugDraw
{
public:

	//DebugDraw();

	virtual ~DebugDraw() {};

	virtual void DrawCircle(const Vector2& center, float radius, const Vector2& axis, const Color& color) = 0;

	virtual void DrawSegment(const Vector2& v1, const Vector2& v2, const Color& color) = 0;

	virtual void DrawPolygon(const Vector2* vertices, int vertexCount, const Color& color) = 0;
};