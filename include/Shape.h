#pragma once

enum ShapeType
{
	CIRCLE,
	POLYGON
};

class Shape
{
public:

	virtual ~Shape() = default;
	virtual Shape* Clone() const = 0;
	virtual ShapeType GetType() const = 0;
};

class Circle : public Shape
{
public:

	Circle(const float radius);
	virtual ~Circle();
	Shape* Clone() const override;
	ShapeType GetType() const override;

public:

	float m_radius;
};

class Polygon : public Shape
{
	Polygon();
};