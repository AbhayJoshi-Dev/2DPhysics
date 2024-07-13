#include<iostream>

#include"Scene.h"
#include"Body.h"

Body::Body(Shape* shape, Vector2 position, BodyType bodytype, Scene* scene) :
	m_shape(shape->Clone()),
	m_position(position),
	m_velocity(0.f, 0.f),
	m_orientation(0.f),
	m_angular_velocity(0.f),
	m_force(0.f, 0.f),
	m_torque(0.f),
	m_bodytype(bodytype),
	m_scene(scene)
{
	m_shape->m_body = this;

	m_shape->ComputeMass(m_material.m_density);

	if (m_bodytype == staticBody)
	{
		m_mass_data.mass = 0.f;
		m_mass_data.inverse_mass = 0.f;

		m_mass_data.inertia = 0.f;
		m_mass_data.inverse_inertia = 0.f;

		m_sweepPos = m_position;
	}

	m_shape->ComputeAABB(&m_proxy.m_aabb, m_position, Mat22(m_orientation));
	m_proxy.m_proxyId = m_scene->bp.CreateProxy(m_proxy.m_aabb, this);

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

	//update sweep data
	m_sweepPos = m_position;
	m_sweepOri = m_orientation;

	m_position += m_velocity * dt;
	m_orientation += m_angular_velocity * dt;


	m_force = { 0.f, 0.f };
	m_torque = 0;
}

void Body::ApplyImpule(const Vector2& impulse, const Vector2& contact_vector)
{
	m_velocity += impulse * m_mass_data.inverse_mass;
	m_angular_velocity += m_mass_data.inverse_inertia * Cross(contact_vector, impulse);
}

BodyType Body::GetType() const
{
	return m_bodytype;
}

//create a extended AABB for broadPhase
void Body::SyncAABB()
{
	AABB aabb1;
	AABB aabb2;

	m_shape->ComputeAABB(&aabb1, m_sweepPos, Mat22(m_sweepOri));
	m_shape->ComputeAABB(&aabb2, m_position, Mat22(m_orientation));

	//combine both AABB
	//AABB bodyAABB;

	m_proxy.m_aabb.Union(aabb1, aabb2);
	//combining aabb
	//bodyAABB.lowerBound = Vector2(std::min(aabb1.lowerBound.x, aabb2.lowerBound.x), std::min(aabb1.lowerBound.y, aabb2.lowerBound.y));
	//bodyAABB.upperBound = Vector2(std::max(aabb1.upperBound.x, aabb2.upperBound.x), std::max(aabb1.upperBound.y, aabb2.upperBound.y));


	//Calculate displacement
	Vector2 displacement = aabb2.GetCenter() - aabb1.GetCenter();


	m_scene->bp.MoveProxy(m_proxy.m_proxyId, m_proxy.m_aabb, displacement);
}