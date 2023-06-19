#include"Scene.h"
#include"Manifold.h"

#include<iostream>

Scene::Scene()
{

}

void Print(int a)
{
	std::cout << a << std::endl;
}

void Scene::Update(const float dt)
{

	for (int i = 0; i < m_bodies.size(); i++)
	{
		Body *A = m_bodies[i];

		for (int j = i + 1; j < m_bodies.size(); j++)
		{
			Body* B = m_bodies[j];

			Manifold manifold(A, B);
			manifold.Solve();
			if (manifold.m_is_contact)
				m_contacts.emplace_back(manifold);
		}
	}


	for (int i = 0; i < m_bodies.size(); i++)
	{
		m_bodies[i]->AddForce({0, 5000.f});//gravity
		m_bodies[i]->IntegrateForces(dt);
	}

	for (int i = 0; i < m_contacts.size(); i++)
		m_contacts[i].ResolveCollision();

	for (int i = 0; i < m_bodies.size(); i++)
		m_bodies[i]->IntegrateVelocities(dt);
}

void Scene::AddBody(Body* body)
{
	m_bodies.push_back(body);
}

std::vector<Body*>& Scene::GetBodies()
{
	return m_bodies;
}