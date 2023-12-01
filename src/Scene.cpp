#include"Scene.h"
#include"Manifold.h"

#include<iostream>

Scene::Scene()
{

}

void Scene::Update(const float dt)
{

	m_contacts.clear();

	for (int i = 0; i < m_bodies.size(); i++)
	{
		Body *A = m_bodies[i];

		for (int j = i + 1; j < m_bodies.size(); j++)
		{
			Body* B = m_bodies[j];

			Manifold manifold(A, B);
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