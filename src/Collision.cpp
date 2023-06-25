#include"Collision.h"
#include"Shape.h"
#include"Body.h"
#include"Manifold.h"
#include"Debug_Draw.h"

#include<iostream>

void Collision::CircleToCircle(Manifold* m, Body* a, Body* b)
{
	Circle* A = (Circle*)a->m_shape;
	Circle* B = (Circle*)b->m_shape;

	Vector2 normal = b->m_position - a->m_position;

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
		m->m_contacts[0] = m->m_normal * A->m_radius + a->m_position;
		Debug_Draw::GetInstance().DrawSegment(b->m_position, a->m_position);
	}
}

void Collision::AABBToAABB(Manifold* m, Body* a, Body* b)
{
	AABB* A = (AABB*)a->m_shape;
	AABB* B = (AABB*)b->m_shape;

	Vector2 normal = b->m_position - a->m_position;

	float x_overlap = (A->m_width + B->m_width) / 2 - std::abs(normal.x);

	if (x_overlap > 0)
	{
		float y_overlap = (A->m_height + B->m_height) / 2 - std::abs(normal.y);

		if (y_overlap > 0)
		{
			if (x_overlap > y_overlap)
			{
				if (normal.y < 0)
					m->m_normal = Vector2(0, -1);
				else
					m->m_normal = Vector2(0, 1);

				m->m_penetration = y_overlap;
				m->m_is_contact = 1;
				return;
			}
			else
			{
				if (normal.x < 0)
					m->m_normal = Vector2(-1, 0);
				else
					m->m_normal = Vector2(1, 0);

				m->m_penetration = x_overlap;
				m->m_is_contact = 1;
				return;
			}
		}
	}
}