#pragma once

class Manifold;
class Body;

class Collision
{
public:

	static void CircleToCircle(Manifold* m, Body* a, Body* b);
};