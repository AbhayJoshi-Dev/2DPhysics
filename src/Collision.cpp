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

	float dist_sq = normal.x * normal.x + normal.y * normal.y;
	
	float sum_radius = A->m_radius + B->m_radius;

	if (dist_sq > sum_radius * sum_radius)
	{
		m->m_is_contact = 0;
		return;
	}

	float dist = std::sqrt(dist_sq);

	if (dist != 0)
	{
		m->m_penetration = sum_radius - dist;
		m->m_normal = normal / dist;
		m->m_is_contact = 1;
		m->m_contacts[0] = m->m_normal * A->m_radius + a->m_position;
		//Debug_Draw::GetInstance().DrawSegment(b->m_position, a->m_position);
	}
}

void Collision::AABBToAABB(Manifold* m, Body* a, Body* b)
{
	AABB* A = (AABB*)a->m_shape;
	AABB* B = (AABB*)b->m_shape;

	Vector2 vec_a_b = b->m_position - a->m_position;

	float x_overlap = (A->m_width + B->m_width) / 2 - std::abs(vec_a_b.x);

	if (x_overlap > 0)
	{
		float y_overlap = (A->m_height + B->m_height) / 2 - std::abs(vec_a_b.y);

		if (y_overlap > 0)
		{
			Debug_Draw::GetInstance().DrawSegment(b->m_position, a->m_position);
			if (x_overlap > y_overlap)
			{
				if (vec_a_b.y < 0)
					m->m_normal = Vector2(0, -1);
				else
					m->m_normal = Vector2(0, 1);

				m->m_penetration = y_overlap;
				m->m_is_contact = 1;
				return;
			}
			else
			{
				if (vec_a_b.x < 0)
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

void Collision::AABBToCircle(Manifold* m, Body* a, Body* b)
{
	AABB* A = (AABB*)a->m_shape;
	Circle* B = (Circle*)b->m_shape;

	Vector2 vec_a_b = b->m_position - a->m_position;


	Vector2 closest = vec_a_b;

	float x_extent = A->m_width / 2;
	float y_extent = A->m_height / 2;

	closest.x = SDL_clamp(closest.x, -x_extent, x_extent);
	closest.y = SDL_clamp(closest.y, -y_extent, y_extent);
	
	bool inside = false;

	if (vec_a_b == closest)
	{
		inside = true;

		if (std::abs(vec_a_b.x) > std::abs(vec_a_b.y))
		{
			if (vec_a_b.x > 0)
				closest.x = x_extent;
			else
				closest.x = -x_extent;
		}
		else
		{
			if (vec_a_b.y > 0)
				closest.y = y_extent;
			else
				closest.y = -y_extent;
		}
	}

	Vector2 normal = vec_a_b - closest;
	float dist_sq = normal.x * normal.x + normal.y * normal.y;
	float radius = B->m_radius;

	if (dist_sq > radius * radius)
		return;

	Debug_Draw::GetInstance().DrawPoint(a->m_position + closest);

	float dist = std::sqrt(dist_sq);

	if (inside)
	{
		m->m_normal = normal * -1;
		m->m_penetration = radius - dist;
		m->m_is_contact = 1;
	}
	else
	{
		m->m_normal = normal;
		m->m_penetration = radius - dist;
		m->m_is_contact = 1;
	}
}

void Collision::CircleToAABB(Manifold* m, Body* a, Body* b)
{
	AABBToCircle(m, b, a);
	m->m_normal = m->m_normal * -1;
}