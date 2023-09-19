#pragma once

#include "MathUtils.h"

class Body;

#define MAX_POLYGON_VERTICES 8

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

public:

	Vector2 m_vertices[MAX_POLYGON_VERTICES];
	Vector2 m_normals[MAX_POLYGON_VERTICES];
	int m_count;
};

class AABB : public Shape
{
public:

	AABB(float width, float height);
	Shape* Clone() const override;
	ShapeType GetType() const override;
	void ComputeMass(float density) override;

public:
	float m_width;
	float m_height;

private:

	Vector2 m_min;
	Vector2 m_max;
};