#include"Manifold.h"
#include"Collision.h"

#include<iostream>

Manifold::Manifold(Body* A, Body* B):
	_A(A),
	_B(B)
{

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
}

void Manifold::ResolveCollision()
{
	// calculate relative velocity
	Vector2 rel_vel_ab = _B->m_velocity - _A->m_velocity;

	// calculate relative velocity along the normal
	float vel_n = rel_vel_ab.Dot(m_normal);

	// do not resolve if velocities are seperating
	if (vel_n > 0)
		return;

	// restitution
	float e = 0.3f;

	// j is the impulse 
	float j = -(1 + e) * vel_n;
	j /= m_normal.Dot(m_normal) * (1 / _A->m_mass_data.mass + 1 / _B->m_mass_data.mass); //not sure about m_normal.Dot(m_normal) multiplication

	// apply impulse
	Vector2 impulse = m_normal * j;

	if(!_A->m_is_static)
		_A->m_velocity -= impulse / _A->m_mass_data.mass;
	if(!_B->m_is_static)
		_B->m_velocity += impulse / _B->m_mass_data.mass;
}

void Manifold::PositionalCorrection()
{
	const float percent = 0.3f;
	const float slop = 0.05f;

	Vector2 correction = m_normal * percent * (std::max(m_penetration - slop, 0.f) / (_A->m_mass_data.inverse_mass + _B->m_mass_data.inverse_mass));
	_A->m_position -= correction * _A->m_mass_data.inverse_mass;
	_B->m_position += correction * _B->m_mass_data.inverse_mass;
}