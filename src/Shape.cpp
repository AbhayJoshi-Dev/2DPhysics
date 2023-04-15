#include"Shape.h"

Circle::Circle(const float radius):
	m_radius(radius)
{}

Circle::~Circle()
{

}

Shape* Circle::Clone() const
{
	return new Circle(m_radius);
}

ShapeType Circle::GetType() const
{
	return CIRCLE;
}

