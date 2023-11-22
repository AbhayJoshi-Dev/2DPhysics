#pragma once

#include"Body.h"

class Manifold
{
public:

	Manifold(Body* A,Body* B);
	void Solve();
	void ResolveCollision();
	void PositionalCorrection();

public:
	
	Body* _A;
	Body* _B;

	int  m_contactCount;
	float m_penetration;
	Vector2 m_normal;
	Vector2 m_contacts[2];
};