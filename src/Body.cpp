#include"Body.h"

Body::Body(Shape *shape, Vector2 position):
	m_shape(shape),
	m_position(position),
	m_velocity(1.f, 1.4f)
{}