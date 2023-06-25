#pragma once

class Manifold;
class Body;

class Collision
{
public:

	static void CircleToCircle(Manifold* m, Body* a, Body* b);
	static void AABBToAABB(Manifold* m, Body* a, Body* b);
};