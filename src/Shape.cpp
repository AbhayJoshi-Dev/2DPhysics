#include"Shape.h"
#include"Body.h"

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

void Circle::ComputeMass(float density)
{
	m_body->m_mass_data.mass = 3.14f * m_radius * m_radius * density;
	m_body->m_mass_data.inverse_mass = (m_body->m_mass_data.mass) ? 1.f / m_body->m_mass_data.mass : 0.f;
}

