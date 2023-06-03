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
		}
	}


	for (auto& body : m_bodies)
	{
		body->AddForce({0, 10000.f});//gravity

		body->IntegrateForces(dt);
		body->IntegrateVelocities(dt);
	}
}

void Scene::AddBody(Body* body)
{
	m_bodies.push_back(body);
}

std::vector<Body*>& Scene::GetBodies()
{
	return m_bodies;
}