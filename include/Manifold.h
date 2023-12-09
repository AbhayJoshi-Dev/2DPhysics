#pragma once

#include"Body.h"

class DebugDraw;

class Manifold
{
public:

	Manifold(Body* A,Body* B);
	void Solve();
	void ResolveCollision();
	void PositionalCorrection();
	void SetDebugDraw(DebugDraw* debugDraw);

public:
	
	Body* m_a;
	Body* m_b;

	int  m_contactCount;
	float m_penetration;
	Vector2 m_normal;
	Vector2 m_contacts[2];

	DebugDraw* m_debugDraw;
};