#pragma once

#include "MathUtils.h"

class Body;

#define MAX_POLYGON_VERTICES 8

struct AABB
{
	Vector2 upperBound;
	Vector2 lowerBound;

	Vector2 GetCenter()
	{
		return 0.5f * (upperBound + lowerBound);
	}

	bool Contains(const AABB& aabb) const
	{
		bool result = true;
		result = result && lowerBound.x <= aabb.lowerBound.x;
		result = result && lowerBound.y <= aabb.lowerBound.y;
		result = result && aabb.upperBound.x <= upperBound.x;
		result = result && aabb.upperBound.y <= upperBound.y;
		return result;
	}

	float GetPerimeter() const
	{
		float l = upperBound.x - lowerBound.x;
		float b = upperBound.y - lowerBound.y;
		return 2.0f * (l + b);
	}

	void Union(AABB A, AABB B)
	{
		//C = (A U B)
		lowerBound = Vector2(std::min(A.lowerBound.x, B.lowerBound.x), std::min(A.lowerBound.y, B.lowerBound.y));
		upperBound = Vector2(std::max(A.upperBound.x, B.upperBound.x), std::max(A.upperBound.y, B.upperBound.y));
	}
};

enum ShapeType
{
	CIRCLE,
	POLYGON,
	AABB_
};

class Shape
{
public:

	virtual ~Shape() = default;
	virtual Shape* Clone() const = 0;
	virtual ShapeType GetType() const = 0;
	virtual void ComputeMass(float density) = 0;
	virtual void ComputeAABB(AABB* aabb, Vector2 position, Mat22 orientation) const = 0;

public:

	Body* m_body;
};

class Circle : public Shape
{
public:

	Circle(const float radius);
	Shape* Clone() const override;
	ShapeType GetType() const override;
	void ComputeMass(float density) override;
	void ComputeAABB(AABB* aabb, Vector2 position, Mat22 orientation) const override;

public:

	float m_radius;
};

class Polygon : public Shape
{
public:

	Polygon() = default;
	Shape* Clone() const override;
	ShapeType GetType() const override;
	void ComputeMass(float density) override;
	void Set(Vector2* vertices, int count);
	Vector2 GetSupport(const Vector2& direction) const;
	void ComputeAABB(AABB* aabb, Vector2 position, Mat22 orientation) const override;

public:

	Vector2 m_vertices[MAX_POLYGON_VERTICES];
	Vector2 m_normals[MAX_POLYGON_VERTICES];
	int m_count;
	Vector2 m_centroid;
};
