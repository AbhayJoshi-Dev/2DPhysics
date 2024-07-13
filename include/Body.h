#pragma once

#include"Shape.h"
class Scene;
class BroadPhase;

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

enum BodyType
{
	staticBody = 0,
	dynamicBody
};

//proxy is just an aabb with an id
struct Proxy
{
	AABB m_aabb;
	int m_proxyId;
};

class Body
{
public:

	Body(Shape *shape, Vector2 position, BodyType bodytype, Scene* scene = nullptr);
	void AddForce(const Vector2& force);
	void IntegrateForces(const float dt);
	void IntegrateVelocities(const float dt);
	void ApplyImpule(const Vector2& impulse, const Vector2& contact_vector);
	BodyType GetType() const;
	void SyncAABB();

public:
	MassData m_mass_data;
	Shape *m_shape;
	Vector2 m_velocity;
	Vector2 m_position;

	float m_orientation;
	float m_angular_velocity;
	Proxy m_proxy;

private:
	float m_torque;
	Material m_material;
	Vector2 m_force;
	BodyType m_bodytype;
	//float m_gravity_scale;
	// 
	//store sweep data
	Vector2 m_sweepPos;
	float m_sweepOri;
	Scene* m_scene;
};