#pragma once
#include<SDL.h>
#include<iostream>

#include"Manifold.h"
#include"Debug_Draw.h"


namespace Collision
{
	void CircleToCircle(Manifold* m, Body* a, Body* b)
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


	void AABBToAABB(Manifold* m, Body* a, Body* b)
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


	void AABBToCircle(Manifold* m, Body* a, Body* b)
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


	void CircleToAABB(Manifold* m, Body* a, Body* b)
	{
		AABBToCircle(m, b, a);
		m->m_normal = m->m_normal * -1;
	}

	void FindIncidentFace(Vector2* v, Polygon* referencePoly, Polygon* incidentPoly, int referenceIndex)
	{
		Vector2 referenceNormal = referencePoly->m_normals[referenceIndex];

		Mat22 rot_ref(referencePoly->m_body->m_orientation);
		Mat22 rot_inc(incidentPoly->m_body->m_orientation);

		// Compute the normal of the reference face in incidentPolygon's frame.
		referenceNormal = rot_ref * referenceNormal; // to world space 
		referenceNormal = rot_inc.Transpose() * referenceNormal; //to incident poly's local space

		int incidentFaceIndex = 0;
		float minDot = FLT_MAX;

		for (int i = 0; i < incidentPoly->m_count; i++)
		{
			float dot = Dot(referenceNormal, incidentPoly->m_normals[i]);

			if (dot < minDot)
			{
				minDot = dot;
				incidentFaceIndex = i;
			}
		}

		// Assign face vertices for incidentFace
		v[0] = rot_inc * incidentPoly->m_vertices[incidentFaceIndex] + incidentPoly->m_body->m_position;
		incidentFaceIndex = incidentFaceIndex + 1 >= incidentPoly->m_count ? 0 : incidentFaceIndex + 1;
		v[1] = rot_inc * incidentPoly->m_vertices[incidentFaceIndex] + incidentPoly->m_body->m_position;
	}
	
	float FindLeastPenetration(int *face, Polygon* A, Polygon* B)
	{
		float best_distance = -FLT_MAX;
		int best_index = 0;;

		Mat22 rotA(A->m_body->m_orientation);
		Mat22 rotB(B->m_body->m_orientation);

		for (int i = 0; i < A->m_count; i++)
		{
			//face normal from A
			Vector2 n = A->m_normals[i];
			Vector2 n_rotA = rotA * n;


			// Transform face normal into B's local space
			Mat22 rotB_T = rotB.Transpose();
			n = rotB_T * n_rotA;

			// Calculate support point from B along -n
			Vector2 support = B->GetSupport(-n);


			// vertex on face from A, transform into
			// B's local space
			Vector2 v = A->m_vertices[i];
			v = rotA * v + A->m_body->m_position;
			v -= B->m_body->m_position;
			v = rotB_T * v;


			// Calculate penetration distance 
			// B's local space
			float d = Dot(n, support - v);

			if (d > best_distance)
			{
				best_distance = d;
				best_index = i;
			}

		}

		*face = best_index;
		return best_distance;
	}

	void PolygonToPolygon(Manifold* m, Body* a, Body* b)
	{
		Polygon* A = (Polygon*)a->m_shape;
		Polygon* B = (Polygon*)b->m_shape;

		int face_A;
		float penetration_A = FindLeastPenetration(&face_A, A, B);

		if (penetration_A >= 0.f)
			return;

		int face_B;
		float penetration_B = FindLeastPenetration(&face_B, B, A);

		if (penetration_B >= 0.f)
			return;

		std::cout << "Collision!" << std::endl;


		int referenceIndex;
		bool flip;

		Polygon* referencePolygon;
		Polygon* incidentPolygon;

		const float k_tol = 0.1f * 0.0005f;

		//find which polygon contains reference face
		if (penetration_B > penetration_A + k_tol)
		{
			referencePolygon = A;
			incidentPolygon = B;

			referenceIndex = face_A;
			flip = false;
		}
		else
		{
			referencePolygon = B;
			incidentPolygon = A;

			referenceIndex = face_B;
			flip = true;
		}


		Vector2 incidentface[2];

		FindIncidentFace(incidentface, referencePolygon, incidentPolygon, referenceIndex);

		Debug_Draw::GetInstance().DrawSegment(incidentface[0], incidentface[1]);
	}

}