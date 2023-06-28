#pragma once

#include"Shape.h"
#include"Vector2.h"

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

public:
	MassData m_mass_data;
	Shape *m_shape;
	Vector2 m_velocity;
	Vector2 m_position;
	bool m_is_static;

private:
	float m_orientation;
	Material m_material;
	Vector2 m_force;
	//float m_gravity_scale;
};