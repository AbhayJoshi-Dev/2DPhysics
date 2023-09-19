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

	Vector2 vec_a_to_b = b->m_position - a->m_position;

	float dist_sq = vec_a_to_b.LengthSquared();
	
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
		m->m_normal = vec_a_to_b.Normalize();
		m->m_is_contact = 1;
		m->m_contacts[0] = a->m_position + m->m_normal * (A->m_radius - m->m_penetration);
		Debug_Draw::GetInstance().DrawSegment(a->m_position, m->m_contacts[0]);
	}
}

void Collision::AABBToAABB(Manifold* m, Body* a, Body* b)
{
	AABB* A = (AABB*)a->m_shape;
	AABB* B = (AABB*)b->m_shape;

	Vector2 vec_a_b = b->m_position - a->m_position;

	float half_width_sum = (A->m_width + B->m_width) / 2;

	if (half_width_sum > std::abs(vec_a_b.x))
	{
		float half_height_sum = (A->m_height + B->m_height) / 2;

		if (half_height_sum > std::abs(vec_a_b.y))
		{

			float penetration_x = half_width_sum - std::abs(vec_a_b.x);
			float penetration_y = half_height_sum - std::abs(vec_a_b.y);


			if (penetration_y < penetration_x)
			{
				if (vec_a_b.y < 0)
					m->m_normal = Vector2(0, -1);
				else
					m->m_normal = Vector2(0, 1);

				m->m_penetration = penetration_y;
				m->m_is_contact = 1;
				return;
			}
			else
			{
				if (vec_a_b.x < 0)
					m->m_normal = Vector2(-1, 0);
				else
					m->m_normal = Vector2(1, 0);

				m->m_penetration = penetration_x;
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

	Vector2 AABB_size(A->m_width / 2, A->m_height / 2);

	Vector2 delta_a_b = b->m_position - a->m_position;

	Vector2 closest_point(SDL_clamp(delta_a_b.x, -AABB_size.x, AABB_size.x),
						  SDL_clamp(delta_a_b.y, -AABB_size.y, AABB_size.y));

	// Vector from closest point to delta_a_b vector
	Vector2 n = delta_a_b - closest_point;

	float dist_sq = n.LengthSquared();
	float radius = B->m_radius;

	if (dist_sq > radius * radius)
		return;

	float dist = std::sqrt(dist_sq);
	m->m_normal = n.Normalize();
	m->m_penetration = radius - dist;

	m->m_is_contact = 1;
	m->m_contacts[0] = -m->m_normal * radius + b->m_position;
}

/*void Collision::AABBToCircle(Manifold* m, Body* a, Body* b)
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

	//Debug_Draw::GetInstance().DrawSegment(a->m_position, a->m_position + closest);

	float dist = std::sqrt(dist_sq);

	if (inside)
	{
		m->m_normal = normal / dist * -1;
		m->m_penetration = radius - dist;
		m->m_is_contact = 1;
		//m->m_contacts[0] = vec_a_b - normal + a->m_position;
	}
	else
	{
		m->m_normal = normal / dist;
		m->m_penetration = radius - dist;
		m->m_is_contact = 1;

		m->m_contacts[0] = vec_a_b - normal + a->m_position;

		Debug_Draw::GetInstance().DrawSegment(m->m_contacts[0], a->m_position);

	}
}*/

void Collision::CircleToAABB(Manifold* m, Body* a, Body* b)
{
	AABBToCircle(m, b, a);
	m->m_normal = m->m_normal * -1;
}