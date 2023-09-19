#pragma once

#include<math.h>

struct Vector2
{
	Vector2() = default;

	Vector2(float x_, float y_) : x(x_), y(y_) {}

	float Length() const { return sqrtf(x * x + y * y); }

	float LengthSquared() const { return x * x + y * y; }

	Vector2 operator -() const { return Vector2(-x, -y); }

	void operator += (const Vector2& v) { x += v.x; y += v.y; }

	void operator -= (const Vector2& v) { x -= v.x; y -= v.y; }

	void operator *= (const Vector2& v) { x *= v.x; y *= v.y; }

	Vector2 Normalize()
	{
		float length = Length();
		if (length != 0)
		{
			x /= length;
			y /= length;
		}

		return *this;
	}



	float x, y;
};

inline Vector2 Cross(const Vector2& a, float s)
{
	return Vector2(-s * a.y, s * a.x);
}

inline float Cross(const Vector2& a, const Vector2& b)
{
	return a.x * b.y - a.y * b.x;
}

inline float Dot(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline Vector2 operator + (const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator - (const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator * (const Vector2& a, float s)
{
	return Vector2(a.x * s, a.y * s);
}