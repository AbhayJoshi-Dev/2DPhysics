#include"Manifold.h"
#include"Collision.h"

#include"DebugDraw.h"
#include<iostream>

//#define HALT __asm { int 3 } for win32

Manifold::Manifold(Body* A, Body* B):
	m_a(A),
	m_b(B)
{
	m_contactCount = 0;
}

void Manifold::Solve()
{
	if(m_a->m_shape->GetType() == CIRCLE && m_b->m_shape->GetType() == CIRCLE)
		Collision::CircleToCircle(this, m_a, m_b);
	else if (m_a->m_shape->GetType() == AABB_ && m_b->m_shape->GetType() == AABB_)
		Collision::AABBToAABB(this, m_a, m_b);

	//AABBtoCircle
	if (m_a->m_shape->GetType() == AABB_ && m_b->m_shape->GetType() == CIRCLE)
		Collision::AABBToCircle(this, m_a, m_b);

	//CircletoAABB
	if (m_a->m_shape->GetType() == CIRCLE && m_b->m_shape->GetType() == AABB_)
		Collision::CircleToAABB(this, m_a, m_b);

	//PolygontoPolygon
	if (m_a->m_shape->GetType() == POLYGON && m_b->m_shape->GetType() == POLYGON)
		Collision::PolygonToPolygon(this, m_a, m_b);

	//CircleToPolygon
	if (m_a->m_shape->GetType() == CIRCLE && m_b->m_shape->GetType() == POLYGON)
		Collision::CircleToPolygon(this, m_a, m_b);

	//PolygonToCircle
	if (m_a->m_shape->GetType() == POLYGON && m_b->m_shape->GetType() == CIRCLE)
		Collision::PolygonToCircle(this, m_a, m_b);
}

void Manifold::ResolveCollision()
{
	for (int i = 0; i < m_contactCount; i++)
	{
		// vector from body's center of mass to collision contact point
		Vector2 rel_a = m_contacts[i] - m_a->m_position;
		Vector2 rel_b = m_contacts[i] - m_b->m_position;


		// calculate relative velocity
		Vector2 rel_vel_ab = m_b->m_velocity + Cross(rel_b, m_b->m_angular_velocity) - m_a->m_velocity - Cross(rel_a, m_a->m_angular_velocity);

		// calculate relative velocity along the normal
		float vel_n = Dot(rel_vel_ab, m_normal);

		//Debug vectors
		/*if (m_a->m_shape->GetType() == CIRCLE && m_b->m_shape->GetType() == CIRCLE)
		{

			Vector2 rel = Vector2(-10.f, 0.f) + Cross(rel_b, 1.f) - Vector2(10.f, 0.f) - Cross(rel_a, 1.f);


			m_debugDraw->DrawSegment(m_a->m_position, Vector2(10.f, 0.f) + Cross(rel_a, 1.f) + m_a->m_position, Color(0, 0, 255, 255));
			m_debugDraw->DrawSegment(m_b->m_position, Vector2(-10.f, 0.f) + Cross(rel_b, 1.f) + m_b->m_position, Color(0, 0, 255, 255));

			float n = Dot(rel, m_normal);

			std::cout << n << std::endl;

			m_debugDraw->DrawSegment(m_a->m_position, rel + m_a->m_position, Color(255, 255, 255, 255));
			m_debugDraw->DrawSegment(m_a->m_position, m_normal * 100.f + m_a->m_position, Color(255, 0, 0, 255));
			
		}*/

		// do not resolve if velocities are seperating
		if (vel_n > 0)
			return;

		// restitution
		float e = 0.5f;

	//	if (m_a->m_shape->GetType() == CIRCLE && m_b->m_shape->GetType() == CIRCLE)
		//{
		//	m_debugDraw->DrawSegment(m_a->m_position, rel_a + m_a->m_position, Color(255, 255, 255, 255));
		//	m_debugDraw->DrawSegment(m_a->m_position, m_normal * 100.f + m_a->m_position, Color(255, 0, 0, 255));
		//}

		//mass sum inverse
		float rel_a_cross_n = Cross(rel_a, m_normal);
		float rel_b_cross_n = Cross(rel_b, m_normal);
		float inv_mass_sum = m_a->m_mass_data.inverse_mass + m_b->m_mass_data.inverse_mass +
							(rel_a_cross_n * rel_a_cross_n) * m_a->m_mass_data.inverse_inertia + (rel_b_cross_n * rel_b_cross_n) * m_b->m_mass_data.inverse_inertia;




		// j is the impulse magnitude
		float j = -(1 + e) * vel_n;
		//j /= m_normal.Dot(m_normal) * (1 / m_a->m_mass_data.mass + 1 / m_b->m_mass_data.mass); //not sure about m_normal.Dot(m_normal) multiplication
		j /= inv_mass_sum;

		j /= m_contactCount;


		// apply impulse
		Vector2 impulse = m_normal * j;

		m_a->ApplyImpule(-impulse, rel_a);
		m_b->ApplyImpule(impulse, rel_b);

		//Friction Impulse

		//Re-calculate relative velocity
		 rel_vel_ab = m_b->m_velocity + Cross(rel_b, m_b->m_angular_velocity) - m_a->m_velocity - Cross(rel_a, m_a->m_angular_velocity);


		//Calculate  tangent vector
		Vector2 t = rel_vel_ab - (m_normal * Dot(rel_vel_ab, m_normal));
		t.Normalize();


		//Solve jt to apply along friction vector
		float jt = -Dot(rel_vel_ab, t);
		//jt /= (1 / m_a->m_mass_data.mass + 1 / m_b->m_mass_data.mass);
		jt /= inv_mass_sum;

		jt /= m_contactCount;

		float mu = std::sqrt(0.4f * 0.4f + 0.4f * 0.4f);//static friction

		//Coulomb's Law
		// Clamp magnitude of friction and create impulse vector
		Vector2 frictionImpulse;
		if (std::abs(jt) < j * mu)
		{
			frictionImpulse = t * jt;
		}
		else
		{
			float dynamicFriction = std::sqrt(0.3f * 0.3f + 0.3f * 0.3f);

			frictionImpulse = t * -j * dynamicFriction;
		}

		//Apply Friction impulse
		m_a->ApplyImpule(-frictionImpulse, rel_a);
		m_b->ApplyImpule(frictionImpulse, rel_b);
	}
}

void Manifold::PositionalCorrection()
{
	const float percent = 0.3f;
	const float slop = 0.05f;

	Vector2 correction = m_normal * percent * (std::max(m_penetration - slop, 0.f) / (m_a->m_mass_data.inverse_mass + m_b->m_mass_data.inverse_mass));

	m_a->m_position -= correction * m_a->m_mass_data.inverse_mass;
	m_b->m_position += correction * m_b->m_mass_data.inverse_mass;
}

void Manifold::SetDebugDraw(DebugDraw* debugDraw)
{
	m_debugDraw = debugDraw;
}