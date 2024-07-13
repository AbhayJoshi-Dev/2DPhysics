#pragma once
#include<SDL.h>
#include<iostream>

#include"Manifold.h"

namespace Collision
{
	static void CircleToCircle(Manifold* m, Body* a, Body* b)
	{
		Circle* A = (Circle*)a->m_shape;
		Circle* B = (Circle*)b->m_shape;

		Vector2 vec_a_to_b = b->m_position - a->m_position;

		float dist_sq = vec_a_to_b.LengthSquared();

		float sum_radius = A->m_radius + B->m_radius;

		if (dist_sq > sum_radius * sum_radius)
		{
			m->m_contactCount = 0;
			return;
		}

		float dist = std::sqrt(dist_sq);

		if (dist != 0)
		{
			m->m_penetration = sum_radius - dist;
			m->m_normal = vec_a_to_b.Normalize();
			m->m_contactCount = 1;
			m->m_contacts[0] = a->m_position + m->m_normal * (A->m_radius - m->m_penetration);
		}
	}

	static void AABBToAABB(Manifold* m, Body* a, Body* b)
	{
		AABB A;
		AABB B;

		a->m_shape->ComputeAABB(&A, a->m_position, Mat22(a->m_orientation));
		b->m_shape->ComputeAABB(&B, b->m_position, Mat22(b->m_orientation));

		Vector2 vec_a_b = b->m_position - a->m_position;

		float half_width_sum = ((A.upperBound.x - A.lowerBound.x) + (B.upperBound.x - B.lowerBound.x)) / 2;

		if (half_width_sum > std::abs(vec_a_b.x))
		{
			float half_height_sum = ((A.upperBound.y - A.lowerBound.y) + (B.upperBound.y - B.lowerBound.y)) / 2;

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
					m->m_contactCount = 1;
					return;
				}
				else
				{
					if (vec_a_b.x < 0)
						m->m_normal = Vector2(-1, 0);
					else
						m->m_normal = Vector2(1, 0);

					m->m_penetration = penetration_x;
					m->m_contactCount = 1;
					return;
				}
			}
		}
	}

	static bool AABBToAABB(const AABB& a, const AABB& b)
	{
		Vector2 d1, d2;

		d1 = b.lowerBound - a.upperBound;
		d2 = a.lowerBound - b.upperBound;

		if (d1.x > 0.0f || d1.y > 0.0f)
			return false;

		if (d2.x > 0.0f || d2.y > 0.0f)
			return false;

		return true;
	}

	//void AABBToCircle(Manifold* m, Body* a, Body* b)
	//{
	//	AABB* A = (AABB*)a->m_shape;
	//	Circle* B = (Circle*)b->m_shape;

	//	Vector2 AABB_size(A->m_width / 2, A->m_height / 2);

	//	Vector2 delta_a_b = b->m_position - a->m_position;

	//	Vector2 closest_point(SDL_clamp(delta_a_b.x, -AABB_size.x, AABB_size.x),
	//		SDL_clamp(delta_a_b.y, -AABB_size.y, AABB_size.y));

	//	// Vector from closest point to delta_a_b vector
	//	Vector2 n = delta_a_b - closest_point;

	//	float dist_sq = n.LengthSquared();
	//	float radius = B->m_radius;

	//	m->m_contactCount = 0;

	//	if (dist_sq > radius * radius)
	//		return;

	//	float dist = std::sqrt(dist_sq);
	//	m->m_normal = n.Normalize();
	//	m->m_penetration = radius - dist;

	//	m->m_contactCount = 1;
	//	m->m_contacts[0] = -m->m_normal * radius + b->m_position;
	//}

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


	/*void CircleToAABB(Manifold* m, Body* a, Body* b)
	{
		AABBToCircle(m, b, a);
		m->m_normal = m->m_normal * -1;
	}*/

	static void FindIncidentFace(Vector2* v, Polygon* referencePoly, Polygon* incidentPoly, int referenceIndex)
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
	
	static float FindLeastPenetration(int *face, Polygon* A, Polygon* B)
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

	static int Clip(Vector2 normal, float c, Vector2* face)
	{

		int count = 0;

		Vector2 vOut[2];

		//distance between reference plane vertex and incident vertex along tangent(or side plane normals) 
		float dist1 = Dot(normal, face[0]) - c;
		float dist2 = Dot(normal, face[1]) - c;


		// If the incident plane vertices are behind(or inside reference polygon) the side planes
		if (dist1 <= 0.0f) vOut[count++] = face[0];
		if (dist2 <= 0.0f) vOut[count++] = face[1];

		//If the points are on different sides of the side plane
		if (dist1 * dist2 < 0.0f)
		{
			//Find intersection point of incident edge and side plane
			float interp = dist1 / (dist1 - dist2);
			vOut[count] = face[0] + interp * (face[1] - face[0]);
			count++;
		}

		face[0] = vOut[0];
		face[1] = vOut[1];

		return count;
	}

	static void PolygonToPolygon(Manifold* m, Body* a, Body* b)
	{
		Polygon* A = (Polygon*)a->m_shape;
		Polygon* B = (Polygon*)b->m_shape;

		int face_A;
		float penetration_A = FindLeastPenetration(&face_A, A, B);

		if (penetration_A >= 0.f)
		{
			m->m_contactCount = 0;
			return;
		}

		int face_B;
		float penetration_B = FindLeastPenetration(&face_B, B, A);

		if (penetration_B >= 0.f)
		{
			m->m_contactCount = 0;
			return;
		}


		int referenceIndex;
		bool flip;

		Polygon* referencePolygon;
		Polygon* incidentPolygon;

		const float k_tol = 0.1f * 0.0005f;

		//find which polygon contains reference face
		if (penetration_B > penetration_A + k_tol)
		{
			referencePolygon = B;
			incidentPolygon = A;

			referenceIndex = face_B;
			flip = true;
		}
		else
		{
			referencePolygon = A;
			incidentPolygon = B;

			referenceIndex = face_A;
			flip = false;
		}


		Vector2 incidentface[2];

		FindIncidentFace(incidentface, referencePolygon, incidentPolygon, referenceIndex);


		//setup reference face vertices
		Vector2 r1 = referencePolygon->m_vertices[referenceIndex];
		referenceIndex = referenceIndex + 1 >= referencePolygon->m_count ? 0 : referenceIndex + 1;
		Vector2 r2 = referencePolygon->m_vertices[referenceIndex];

		// transform reference face vertices to world space
		Mat22 rotRef(referencePolygon->m_body->m_orientation);

		r1 = rotRef * r1 + referencePolygon->m_body->m_position;
		r2 = rotRef * r2 + referencePolygon->m_body->m_position;

		//Debug_Draw::GetInstance().DrawSegment(r1, r2);

		//Calculate tangent along reference face (world space)
		Vector2 tangent = (r2 - r1).Normalize();

		//compute normal to reference face
		Vector2 normal = -Cross(tangent, 1.0f);
		//Vector2 normal(tangent.y, -tangent.x);


		// ax + by = c
		// c is distance from origin
		//calculating distance from origin to reference face vertices along normal and tangent
		float frontOffset = Dot(normal, r1);// normal.x * r1.x + normal.y * r1.y = c

		float negSide = -Dot(tangent, r1);
		float posSide = Dot(tangent, r2);



		if (Clip(-tangent, negSide, incidentface) < 2)
			return;

		if (Clip(tangent, posSide, incidentface) < 2)
			return;

		m->m_normal = flip ? -normal : normal;

		m->m_contactCount = 1;
		
		int contactPoint = 0;
		float seperation = Dot(normal, incidentface[0]) - frontOffset;

	//	Debug_Draw::GetInstance().DrawSegment(Vector2(0.f, 0.f), normal * 1000.f);
	//	Debug_Draw::GetInstance().DrawSegment(Vector2(0.f, 0.f), incidentface[0]);
	//	Debug_Draw::GetInstance().DrawSegment(r1, normal * frontOffset);
	//	Debug_Draw::GetInstance().DrawSegment(incidentface[0], normal * (seperation + frontOffset));

		if (seperation <= 0.0f)
		{
			m->m_contacts[contactPoint] = incidentface[0];
			m->m_penetration = -seperation;
			contactPoint++;
		}

		seperation = Dot(normal, incidentface[1]) - frontOffset;
		if (seperation <= 0.0f)
		{
			m->m_contacts[contactPoint] = incidentface[1];
			m->m_penetration += -seperation;
			contactPoint++;

			//Average penetration
			m->m_penetration /= contactPoint;
		}

		m->m_contactCount = contactPoint;
	}


	static void CircleToPolygon(Manifold* m, Body* a, Body* b)
	{
		Circle* A = (Circle*)a->m_shape;
		Polygon* B = (Polygon*)b->m_shape;

		Mat22 rotB(b->m_orientation);

		m->m_contactCount = 0;

		//Transform circle's center(world space) to polygon local space
		Vector2 circleCenter = a->m_position;
		circleCenter -= b->m_position;
		circleCenter = rotB.Transpose() * circleCenter;

		// Find the min separating edge
		//calculating the minimum seperation between polygon face and circle's center
		float seperation = -FLT_MAX;
		int faceNormal = 0;
		for (int i = 0; i < B->m_count; i++)
		{
			float d = Dot(B->m_normals[i], circleCenter - B->m_vertices[i]);


			if (d > A->m_radius)
				return;
			
			if (d > seperation)
			{
				seperation = d;
				faceNormal = i;
			}
		}

		//faceNormal's vertices
		Vector2 v1 = B->m_vertices[faceNormal];
		int i2 = faceNormal + 1 < B->m_count ? faceNormal + 1 : 0;
		Vector2 v2 = B->m_vertices[i2];

		//circle center within polygon
		if (seperation < FLT_EPSILON)
		{
			m->m_contactCount = 1;
			m->m_normal = -(rotB * B->m_normals[faceNormal]);
			m->m_contacts[0] = m->m_normal * A->m_radius + a->m_position;
			m->m_penetration = A->m_radius;
			return;
		}


		// Compute barycentric coordinates
		float u1 = Dot(circleCenter - v1, v2 - v1);
		float u2 = Dot(circleCenter - v2, v1 - v2);

		m->m_penetration = A->m_radius - seperation;



		//Debug_Draw::GetInstance().DrawSegment(b->m_position, (circleCenter - v1) + b->m_position);
		//Debug_Draw::GetInstance().DrawSegment(b->m_position, b->m_position + (v2 - v1));
		// 
		//Debug_Draw::GetInstance().DrawSegment(b->m_position, (circleCenter - v2) + b->m_position);
		//Debug_Draw::GetInstance().DrawSegment(b->m_position, b->m_position + (v1 - v2));

		if (u1 <= 0.0f)
		{
			Vector2 v1ToCenter = circleCenter - v1;
			float distSq = v1ToCenter.LengthSquared();
			
			//checking the distance between collision face vertex and circle's center
			if (distSq > A->m_radius * A->m_radius)
				return;
			m->m_contactCount = 1;
			Vector2 n = v1 - circleCenter;
			n = rotB * n;
			m->m_normal = n.Normalize();
			v1 = rotB * v1 + b->m_position;
			m->m_contacts[0] = v1;
		}
		else if(u2 <= 0.0f)
		{
			Vector2 v2ToCenter = circleCenter - v2;
			float distSq = v2ToCenter.LengthSquared();

			//checking the distance between collision face vertex and circle's center
			if (distSq > A->m_radius * A->m_radius)
				return;
			m->m_contactCount = 1;
			Vector2 n = v2 - circleCenter;
			n = rotB * n;
			m->m_normal = n.Normalize();
			v2 = rotB * v2 + b->m_position;
			m->m_contacts[0] = v2;
		}
		else
		{
			Vector2 n = B->m_normals[faceNormal];
			//To check the distance between circle's center and the collision face
			if (Dot(circleCenter - v1, n) > A->m_radius)
				return;
			n = rotB * n;
			m->m_normal = -n;
			m->m_contacts[0] = m->m_normal * A->m_radius + a->m_position;
			m->m_contactCount = 1;
		}

	}

	static void PolygonToCircle(Manifold* m, Body* a, Body* b)
	{
		CircleToPolygon(m, b, a);
		m->m_normal = -m->m_normal;
	}

}