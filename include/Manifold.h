#pragma once

#include"Body.h"

class Manifold
{
public:

	Manifold(Body* A,Body* B);
	void Solve();

public:
	
	Body* _A;
	Body* _B;

	float m_penetration;
	Vector2 m_normal;
};