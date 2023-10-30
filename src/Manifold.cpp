#include"Manifold.h"
#include"Collision.h"

Manifold::Manifold(Body* A, Body* B):
	_A(A),
	_B(B)
{
	m_is_contact = 0;
}

void Manifold::Solve()
{
	if(_A->m_shape->GetType() == CIRCLE && _B->m_shape->GetType() == CIRCLE)
		Collision::CircleToCircle(this, _A, _B);
	else if (_A->m_shape->GetType() == AABB_ && _B->m_shape->GetType() == AABB_)
		Collision::AABBToAABB(this, _A, _B);

	//AABBtoCircle
	if (_A->m_shape->GetType() == AABB_ && _B->m_shape->GetType() == CIRCLE)
		Collision::AABBToCircle(this, _A, _B);

	//CircletoAABB
	if (_A->m_shape->GetType() == CIRCLE && _B->m_shape->GetType() == AABB_)
		Collision::CircleToAABB(this, _A, _B);

	//PolygontoPolygon
	if (_A->m_shape->GetType() == POLYGON && _B->m_shape->GetType() == POLYGON)
		Collision::PolygonToPolygon(this, _A, _B);
}

void Manifold::ResolveCollision()
{
	// vector from collision contact to body center of mass
	Vector2 rel_a = m_contacts[0] - _A->m_position;
	Vector2 rel_b = m_contacts[0] - _B->m_position;


	// counter-clockwise perpendicular Vector 
	//Vector2 rel_a_prep(-rel_a.y, rel_a.x);
	//Vector2 rel_b_prep(-rel_b.y, rel_b.x);


	// calculate relative velocity
	//Vector2 angular_linear_vel_a = rel_a_prep * _A->m_angular_velocity;
	//Vector2 angular_linear_vel_b = rel_b_prep * _B->m_angular_velocity;
	Vector2 rel_vel_ab = _B->m_velocity + Cross(rel_b, _B->m_angular_velocity) - _A->m_velocity - Cross(rel_a, _A->m_angular_velocity);

	// calculate relative velocity along the normal
	float vel_n = Dot(rel_vel_ab, m_normal);

	// do not resolve if velocities are seperating
	if (vel_n > 0)
		return;

	// restitution
	float e = 0.5f;




	//mass sum inverse
	float rel_a_cross_n = Cross(rel_a, m_normal);
	float rel_b_cross_n = Cross(rel_b, m_normal);
	float inv_mass_sum = _A->m_mass_data.inverse_mass + _B->m_mass_data.inverse_mass +
						(rel_a_cross_n * rel_a_cross_n) * _A->m_mass_data.inverse_inertia + (rel_b_cross_n * rel_b_cross_n) * _B->m_mass_data.inverse_inertia;




	// j is the impulse magnitude
	float j = -(1 + e) * vel_n;
	//j /= m_normal.Dot(m_normal) * (1 / _A->m_mass_data.mass + 1 / _B->m_mass_data.mass); //not sure about m_normal.Dot(m_normal) multiplication
	j /= inv_mass_sum;


	// apply impulse
	Vector2 impulse = m_normal * j;

	_A->ApplyImpule(-impulse, rel_a);
	_B->ApplyImpule(impulse, rel_b);

	//Friction Impulse

	//Re-calculate relative velocity
	 //angular_linear_vel_a = rel_a_prep * _A->m_angular_velocity;
	// angular_linear_vel_b = rel_b_prep * _B->m_angular_velocity;
	 rel_vel_ab = _B->m_velocity + Cross(rel_b, _B->m_angular_velocity) - _A->m_velocity - Cross(rel_a, _A->m_angular_velocity);


	//Calculate  tangent vector
	Vector2 t = rel_vel_ab - (m_normal * Dot(rel_vel_ab, m_normal));
	t.Normalize();

	// draw tangent
	//Debug_Draw::GetInstance().DrawSegment(m_contacts[0], m_contacts[0] + t * 10.f);

	//Solve jt to apply along friction vector
	float jt = -Dot(rel_vel_ab, t);
	//jt /= (1 / _A->m_mass_data.mass + 1 / _B->m_mass_data.mass);
	jt /= inv_mass_sum;

	float mu = std::sqrt(0.2f * 0.2f + 0.2f * 0.2f);//static friction

	//Coulomb's Law
	// Clamp magnitude of friction and create impulse vector
	Vector2 friction_impulse;
	if (std::abs(jt) < j * mu)
	{
		friction_impulse = t * jt;
	}
	else
	{
		float dynamic_friction = std::sqrt(0.1f * 0.1f + 0.1f * 0.1f);

		friction_impulse = t * -j * dynamic_friction;
	}

	//Apply Friction impulse
	_A->ApplyImpule(-friction_impulse, rel_a);
	_B->ApplyImpule(friction_impulse, rel_b);

}

void Manifold::PositionalCorrection()
{
	const float percent = 0.3f;
	const float slop = 0.05f;

	Vector2 correction = m_normal * percent * (std::max(m_penetration - slop, 0.f) / (_A->m_mass_data.inverse_mass + _B->m_mass_data.inverse_mass));

	_A->m_position -= correction * _A->m_mass_data.inverse_mass;
	_B->m_position += correction * _B->m_mass_data.inverse_mass;
}