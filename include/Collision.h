#pragma once

class Manifold;
class Body;

class Collision
{
public:

	void CircleToCircle(Manifold* m, Body* a, Body* b);
};