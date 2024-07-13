#pragma once

#include<vector>

#include"BroadPhase.h"

class DebugDraw;
class Manifold;
class Body;

class Scene
{
public:

	Scene();
	~Scene();
	void Step(const float dt);
	void AddBody(Body* body);
	std::vector<Body*>& GetBodies();
	void Draw();
	void SetDebugDraw(DebugDraw* draw);
	void AddPair(void* userData1, void* userData2);
	int GetTotalContacts() const;

public:
	BroadPhase bp;
	bool m_bruteForce;
private:
	std::vector<Body*> m_bodies;
	std::vector<Manifold> m_contacts;
	DebugDraw* m_draw;
};