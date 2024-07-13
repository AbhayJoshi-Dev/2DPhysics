#include"Scene.h"
#include"Manifold.h"
#include"DebugDraw.h"

#include<iostream>
#include<algorithm>

Scene::Scene()
{
	m_draw = nullptr;
	m_bruteForce = false;
}

Scene::~Scene()
{
	for (Body* body : m_bodies)
		delete body;

	m_bodies.clear();
}

void Scene::Step(const float dt)
{



	if (!m_bruteForce)
	{
		bp.GeneratePairs(this);
	
		//remove if aabb is not overlapping
		std::vector<Manifold>::iterator it = std::remove_if(m_contacts.begin(), m_contacts.end(),
			[&](Manifold& m)
			{
				return !(bp.TestOverlap(m.m_a->m_proxy.m_proxyId, m.m_b->m_proxy.m_proxyId));
			});
		if(it != m_contacts.end())
			m_contacts.erase(it, m_contacts.end());

		for (Manifold& manifold : m_contacts)
			manifold.Solve();
	}
	else
	{
		for (int i = 0; i < m_bodies.size(); i++)
		{
			Body* A = m_bodies[i];

			for (int j = i + 1; j < m_bodies.size(); j++)
			{
				Body* B = m_bodies[j];

				if (A == B)
					continue;

				Manifold manifold(A, B);
				manifold.SetDebugDraw(m_draw);
				manifold.Solve();
				if (manifold.m_contactCount)
					m_contacts.emplace_back(manifold);
			}
		}
	}

	// Integrate forces
	for (int i = 0; i < m_bodies.size(); i++)
		m_bodies[i]->IntegrateForces(dt);

	//Resolve Collisions
	for(int j = 0; j < 8; j++)
		for (int i = 0; i < m_contacts.size(); i++)
		{
			if(m_contacts[i].m_contactCount)
				m_contacts[i].ResolveCollision();
		}

	//Integrate Velocities
	for (int i = 0; i < m_bodies.size(); i++)
	{
		m_bodies[i]->IntegrateVelocities(dt);
		m_bodies[i]->SyncAABB();
	}

	// Avoid object sinking by correcting positions
	for (int i = 0; i < m_contacts.size(); i++)
	{
		if (m_contacts[i].m_contactCount)
			m_contacts[i].PositionalCorrection();
	}

	if (m_bruteForce)
		m_contacts.clear();
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

void Scene::AddPair(void* userData1, void* userData2)
{
	Body* bodyA = (Body*)userData1;
	Body* bodyB = (Body*)userData2;

	if (bodyA == bodyB)
		return;

	Manifold manifold(bodyA, bodyB);

	//check if contacts already contains the manifold
	std::vector<Manifold>::iterator it = std::find_if(m_contacts.begin(), m_contacts.end(),
		[&manifold](Manifold& m)
		{
			if (manifold.m_a == m.m_a && manifold.m_b == m.m_b) return true;

			return false;
		});

	if (it != m_contacts.end())
		return;

	manifold.SetDebugDraw(m_draw);
//	manifold.Solve();
//	if (manifold.m_contactCount)
		m_contacts.push_back(manifold);
}

int Scene::GetTotalContacts() const
{
	return m_contacts.size();
}