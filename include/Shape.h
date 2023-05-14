#pragma once

#include"Vector2.h"

class Body;

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
	Polygon();
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