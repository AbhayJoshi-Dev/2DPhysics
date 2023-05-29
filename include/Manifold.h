#pragma once

#include<Body.h>

class Manifold
{
public:
	
	Body* A;
	Body* B;

	float m_penetration;
	Vector2 m_normal;
};