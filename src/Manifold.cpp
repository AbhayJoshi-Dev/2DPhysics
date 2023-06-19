#include"Manifold.h"
#include"Collision.h"

Manifold::Manifold(Body* A, Body* B):
	_A(A),
	_B(B)
{

}

void Manifold::Solve()
{
	if(_A->m_shape->GetType() == CIRCLE && _B->m_shape->GetType() == CIRCLE)
		Collision::CircleToCircle(this, _A, _B);
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
	_A->m_velocity -= impulse / _A->m_mass_data.mass;
	_B->m_velocity += impulse / _B->m_mass_data.mass;
}