#include"Body.h"

Body::Body(const Shape &shape, Vector2 position):
	m_shape(shape.Clone()),
	m_position(position),
	m_velocity(0.f, 0.f)
{
	m_shape->m_body = this;
	m_shape->ComputeMass(m_material.m_density);
}

void Body::AddForce(const Vector2& force)
{
	m_force += force;
}

void Body::IntegrateForces(const float dt)
{
	m_velocity += (m_force * m_mass_data.inverse_mass) * (dt / 2.f);

	m_force = { 0.f, 0.f };
}

void Body::IntegrateVelocities(const float dt)
{
	m_position += m_velocity * dt;
}