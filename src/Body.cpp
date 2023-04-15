#include"Body.h"

Body::Body(const Shape &shape, Vector2 position):
	m_shape(shape.Clone()),
	m_position(position),
	m_velocity(1.f, 1.4f)
{}