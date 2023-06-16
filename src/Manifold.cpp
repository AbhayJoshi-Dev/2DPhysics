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

}