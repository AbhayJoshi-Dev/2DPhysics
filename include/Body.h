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
	float m_density;
	float m_restitution;
};

class Body
{
public:

	Body(Shape *shape, Vector2 position);

private:

	Shape *m_shape;
	Vector2 m_position;
	float m_orientation;
	Material m_material;
	MassData m_mass_data;
	Vector2 m_velocity;
	Vector2 m_force;
	//float m_gravity_scale;
};