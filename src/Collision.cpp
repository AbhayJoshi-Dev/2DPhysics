#include"Collision.h"
#include"Shape.h"
#include"Body.h"
#include"Manifold.h"
#include"Debug_Draw.h"

void Collision::CircleToCircle(Manifold* m, Body* a, Body* b)
{
	Circle* A = (Circle*)a->m_shape;
	Circle* B = (Circle*)b->m_shape;

	Vector2 normal = b->GetPosition() - a->GetPosition();

	float dist = normal.GetLength();
	
	float sum_radius = A->m_radius + B->m_radius;

	if (dist > sum_radius)
	{
		m->m_is_contact = 0;
		return;
	}

	if (dist != 0)
	{
		m->m_penetration = sum_radius - dist;
		m->m_normal = normal / dist;
		m->m_is_contact = 1;
		m->m_contacts[0] = m->m_normal * A->m_radius + a->GetPosition();
		Debug_Draw::GetInstance().DrawSegment(b->GetPosition(), a->GetPosition());
	}
}