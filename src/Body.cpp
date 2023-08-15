#include"Body.h"

#include<iostream>

Body::Body(const Shape &shape, Vector2 position, bool is_static):
	m_shape(shape.Clone()),
	m_position(position),
	m_velocity(0.f, 0.f),
	m_orientation(0.f),
	m_angular_velocity(0.f)
{
	m_shape->m_body = this;
	m_shape->ComputeMass(m_material.m_density);

	if (is_static)
	{
		m_mass_data.mass = 0.f;
		m_mass_data.inverse_mass = 0.f;

		m_mass_data.inertia = 0.f;
		m_mass_data.inverse_inertia = 0.f;
	}

}

void Body::AddForce(const Vector2& force)
{
	m_force += force;
}

void Body::IntegrateForces(const float dt)
{
	// Semi-Implicit Euler Integration (or Symplectic Euler Integration)
	if (m_mass_data.inverse_mass == 0.f)
		return;
	m_velocity += (m_force * m_mass_data.inverse_mass + Vector2(0.f, 500.f)) * (dt / 2.f);
	m_angular_velocity += m_torque * m_mass_data.inverse_inertia * (dt / 2.f);
}

void Body::IntegrateVelocities(const float dt)
{
	if (m_mass_data.inverse_mass == 0.f)
		return;
	m_position += m_velocity * dt;
	m_orientation += m_angular_velocity * dt;


	m_force = { 0.f, 0.f };
	m_torque = 0;
}

void Body::ApplyImpule(const Vector2& impulse, const Vector2& contact_vector)
{
	m_velocity += impulse * m_mass_data.inverse_mass;
	m_angular_velocity += m_mass_data.inverse_inertia * contact_vector.Cross(impulse);
}