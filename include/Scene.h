#pragma once

#include<vector>

#include"Body.h"
#include"Manifold.h"

class DebugDraw;

class Scene
{
public:

	Scene();
	void Step(const float dt);
	void AddBody(Body* body);
	std::vector<Body*>& GetBodies();
	void Draw();
	void SetDebugDraw(DebugDraw* draw);

private:
	std::vector<Body*> m_bodies;
	std::vector<Manifold> m_contacts;
	DebugDraw* m_draw;
};