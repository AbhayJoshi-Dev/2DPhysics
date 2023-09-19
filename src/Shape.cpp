#include<cmath>
#include<iostream>

#include"Shape.h"
#include"Body.h"

Circle::Circle(const float radius):
	m_radius(radius)
{}

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

	m_body->m_mass_data.inertia = m_body->m_mass_data.mass * m_radius * m_radius;
	m_body->m_mass_data.inverse_inertia = (m_body->m_mass_data.inertia) ? 1.f / m_body->m_mass_data.inertia : 0.f;
}

AABB::AABB(float width, float height):
	m_width(width),
	m_height(height)
{}

Shape* AABB::Clone() const
{
	return new AABB(m_width, m_height);
}

ShapeType AABB::GetType() const
{
	return AABB_;
}

void AABB::ComputeMass(float density)
{
	m_body->m_mass_data.mass = (m_width * m_height) * density;
	m_body->m_mass_data.inverse_mass = (m_body->m_mass_data.mass) ? 1.f / m_body->m_mass_data.mass : 0.f;

	m_body->m_mass_data.inertia = m_body->m_mass_data.mass * (m_width * m_width + m_height * m_height) / 12;
	m_body->m_mass_data.inverse_inertia = (m_body->m_mass_data.inertia) ? 1.f / m_body->m_mass_data.inertia : 0.f;
}

void Polygon::Set(Vector2* vertices, int count)
{

}