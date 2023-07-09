#include"Body.h"

#include<iostream>

Body::Body(const Shape &shape, Vector2 position, bool is_static):
	m_shape(shape.Clone()),
	m_position(position),
	m_velocity(0.f, 0.f),
	m_is_static(is_static),
	m_orientation(0.f),
	m_angular_velocity(0.f)
{
	m_shape->m_body = this;
	m_shape->ComputeMass(m_material.m_density);
}

void Body::AddForce(const Vector2& force)
{
	if(!m_is_static)
		m_force += force;
}

void Body::IntegrateForces(const float dt)
{
	// Semi-Implicit Euler Integration (or Symplectic Euler Integration)
	if (!m_is_static)
	{
		m_velocity += (m_force * m_mass_data.inverse_mass + Vector2(0.f, 500.f)) * (dt / 2.f);
		m_angular_velocity += m_torque * m_mass_data.inverse_inertia * (dt / 2.f);
	}
}

void Body::IntegrateVelocities(const float dt)
{
	m_position += m_velocity * dt;
	m_orientation += m_angular_velocity * dt;

	m_force = { 0.f, 0.f };
}