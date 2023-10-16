#pragma once

#include<math.h>
#include<stdlib.h>

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

struct Mat22
{
	Mat22() {}

	Mat22(const Vector2& _col1, const Vector2& _col2) : col1(_col1), col2(_col2) {}

	Mat22(float angle)
	{
		float c = cosf(angle);
		float s = sinf(angle);

		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	Mat22 Transpose() const
	{
		return Mat22(Vector2(col1.x, col2.x), Vector2(col1.y, col2.y));
	}

	Vector2 col1, col2;
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

inline Vector2 operator * (float s, const Vector2& a)
{
	return Vector2(a.x * s, a.y * s);
}

inline Vector2 operator * (const Mat22& A, const Vector2& v)
{
	return Vector2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

inline int Random(int lo, int hi)
{
	int r = rand();
	r = (r % (hi - lo + 1)) + lo;
	return r;
}