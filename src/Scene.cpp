#include"Scene.h"
#include"Manifold.h"
#include"DebugDraw.h"

#include<iostream>

Scene::Scene()
{
	m_draw = nullptr;
}

void Scene::Step(const float dt)
{

	m_contacts.clear();

	for (int i = 0; i < m_bodies.size(); i++)
	{
		Body *A = m_bodies[i];

		for (int j = i + 1; j < m_bodies.size(); j++)
		{
			Body* B = m_bodies[j];

			Manifold manifold(A, B);
			manifold.SetDebugDraw(m_draw);
			manifold.Solve();
			if (manifold.m_contactCount)
				m_contacts.emplace_back(manifold);
		}
	}

	// Integrate forces
	for (int i = 0; i < m_bodies.size(); i++)
		m_bodies[i]->IntegrateForces(dt);

	//Resolve Collisions
	for(int j = 0; j < 8; j++)
		for (int i = 0; i < m_contacts.size(); i++)
			m_contacts[i].ResolveCollision();

	//Integrate Velocities
	for (int i = 0; i < m_bodies.size(); i++)
		m_bodies[i]->IntegrateVelocities(dt);

	// Avoid object sinking by correcting positions
	for (int i = 0; i < m_contacts.size(); i++)
		m_contacts[i].PositionalCorrection();
}

void Scene::AddBody(Body* body)
{
	m_bodies.push_back(body);
}

std::vector<Body*>& Scene::GetBodies()
{
	return m_bodies;
}

void Scene::Draw()
{
	for (auto body : m_bodies)
	{
		if (body->m_shape->GetType() == CIRCLE)
		{
			Circle* circle = (Circle*)body->m_shape;
			float radius = circle->m_radius;

			Mat22 rot(body->m_orientation);

			m_draw->DrawCircle(body->m_position, radius, rot * Vector2(1.f, 0.f), Color(150, 255, 150, 255));
		}
		else if (body->m_shape->GetType() == POLYGON)
		{
			Polygon* poly = (Polygon*)body->m_shape;
			int count = poly->m_count;

			Mat22 rot(body->m_orientation);

			Vector2 vertices[MAX_POLYGON_VERTICES];

			for (int i = 0; i < count; i++)
			{
				vertices[i] = body->m_position + rot * poly->m_vertices[i];
			}

			m_draw->DrawPolygon(vertices, count, Color(150, 255, 150, 255));

		}
	}
}

void Scene::SetDebugDraw(DebugDraw* draw)
{
	m_draw = draw;
}