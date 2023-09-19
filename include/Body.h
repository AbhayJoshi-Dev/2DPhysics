#pragma once

#include"Shape.h"

struct MassData
{
	float mass;
	float inverse_mass;

	float inertia;
	float inverse_inertia;
};

struct Material
{
	float m_density = 1.f;
	float m_restitution;
};

class Body
{
public:

	Body(const Shape &shape, Vector2 position, bool is_static);
	void AddForce(const Vector2& force);
	void IntegrateForces(const float dt);
	void IntegrateVelocities(const float dt);
	void ApplyImpule(const Vector2& impulse, const Vector2& contact_vector);

public:
	MassData m_mass_data;
	Shape *m_shape;
	Vector2 m_velocity;
	Vector2 m_position;
	//bool m_is_static;

	float m_orientation;
	float m_angular_velocity;

private:
	float m_torque;
	Material m_material;
	Vector2 m_force;
	//float m_gravity_scale;
};