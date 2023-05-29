#include"Scene.h"

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