#pragma once

#include<vector>

#include"Body.h"

class Scene
{
public:

	Scene();
	void Update(const float dt);
	void AddBody(Body* body);
	std::vector<Body*>& GetBodies();

private:
	std::vector<Body*> m_bodies;
};