#include<cmath>
#include<iostream>

#include"Shape.h"
#include"Body.h"

Circle::Circle(const float radius):
	m_radius(radius)
{}

Shape* Circle::Clone() const
{
	return new Circle(m_radius);
}

ShapeType Circle::GetType() const
{
	return CIRCLE;
}

void Circle::ComputeMass(float density)
{
	m_body->m_mass_data.mass = 3.14f * m_radius * m_radius * density;
	m_body->m_mass_data.inverse_mass = (m_body->m_mass_data.mass) ? 1.f / m_body->m_mass_data.mass : 0.f;

	m_body->m_mass_data.inertia = m_body->m_mass_data.mass * m_radius * m_radius;
	m_body->m_mass_data.inverse_inertia = (m_body->m_mass_data.inertia) ? 1.f / m_body->m_mass_data.inertia : 0.f;
}

AABB::AABB(float width, float height):
	m_width(width),
	m_height(height)
{}

Shape* AABB::Clone() const
{
	return new AABB(m_width, m_height);
}

ShapeType AABB::GetType() const
{
	return AABB_;
}

void AABB::ComputeMass(float density)
{
	m_body->m_mass_data.mass = (m_width * m_height) * density;
	m_body->m_mass_data.inverse_mass = (m_body->m_mass_data.mass) ? 1.f / m_body->m_mass_data.mass : 0.f;

	m_body->m_mass_data.inertia = m_body->m_mass_data.mass * (m_width * m_width + m_height * m_height) / 12;
	m_body->m_mass_data.inverse_inertia = (m_body->m_mass_data.inertia) ? 1.f / m_body->m_mass_data.inertia : 0.f;
}

Shape* Polygon::Clone() const
{
	Polygon* polygon = new Polygon();

	for (int i = 0; i < m_count; i++)
	{
		polygon->m_vertices[i] = m_vertices[i];
		polygon->m_normals[i] = m_normals[i];
	}
	polygon->m_count = m_count;

	return polygon;
}

ShapeType Polygon::GetType() const
{
	return POLYGON;
}

void Polygon::ComputeMass(float density)
{
	Vector2 c(0.0f, 0.0f);
	float area = 0.0f;

	float I = 0.f;
	// Get a reference point for forming triangles.
	// Use the first vertex to reduce round-off errors.
	Vector2 s = m_vertices[0];

	const float inv3 = 1.0f / 3.0f;

	for (int i = 0; i < m_count; ++i)
	{
		// Triangle vertices.
		Vector2 p1 = m_vertices[0] - s;
		Vector2 p2 = m_vertices[i] - s;
		Vector2 p3 = i + 1 < m_count ? m_vertices[i + 1] - s : m_vertices[0] - s;

		Vector2 e1 = p2 - p1;
		Vector2 e2 = p3 - p1;

		float D = Cross(e1, e2);

		float triangleArea = 0.5f * D;
		area += triangleArea;

		// Area weighted centroid
		c += triangleArea * inv3 * (p1 + p2 + p3);		

		float intx2 = e1.x * e1.x + e1.x * e2.x + e2.x * e2.x;
		float inty2 = e1.y * e1.y + e1.y * e2.y + e2.y * e2.y;

		I += (0.25f * inv3 * D) * (intx2 + inty2);

	}

	// Centroid
	c = (1.0f / area) * c + s;
	m_centroid = c;

	for (int i = 0; i < m_count; i++)
		m_vertices[i] -= c;

	m_body->m_mass_data.mass = density * area;
	m_body->m_mass_data.inverse_mass = (m_body->m_mass_data.mass) ? 1.0f / m_body->m_mass_data.mass : 0.0f;
	m_body->m_mass_data.inertia = I * density;
	m_body->m_mass_data.inverse_inertia = m_body->m_mass_data.inertia ? 1.0f / m_body->m_mass_data.inertia : 0.0f;
}

// Gift Wrapping Convex Hull Algorithm
void Polygon::Set(Vector2* vertices, int count)
{
	if (count < 2 && count > MAX_POLYGON_VERTICES)
		return;

	//find the left most point on the convex hull
	int left_most_index = 0;
	float left_most_x_coordinate = vertices[0].x;

	for (int i = 1; i < count; i++)
	{
		float x = vertices[i].x;

		if (x < left_most_x_coordinate)
		{
			left_most_x_coordinate = x;
			left_most_index = i;
		}
		else if (x == left_most_x_coordinate)
			if (vertices[i].y < vertices[left_most_index].y)
				left_most_index = i;
	}


	int hull_indexes[MAX_POLYGON_VERTICES];
	//last found hull point index
	int out_count = 0;
	int hull_index = left_most_index;



	for (;;)
	{
		hull_indexes[out_count] = hull_index;


		//Search for next index that wraps around the hull
		// by computing cross products to find the most counter-clockwise vertex in the set

		int next_hull_point_index = 0;

		for (int i = 1; i < count; i++)
		{
			// Skip if same index
			if (next_hull_point_index == hull_index)
			{
				next_hull_point_index = i;
				continue;
			}

			//cross product the set of vertices

			Vector2 v1 = vertices[next_hull_point_index] - vertices[hull_indexes[out_count]];
			Vector2 v2 = vertices[i] - vertices[hull_indexes[out_count]];

			float cross = Cross(v1, v2);

			if (cross < 0.f)
				next_hull_point_index = i;

			// Cross product is zero then vectors are on same line
			// Record vertex farthest along that line
			if (cross == 0.0f && v2.LengthSquared() > v1.LengthSquared())
				next_hull_point_index = i;

		}

		out_count++;
		hull_index = next_hull_point_index;

		//End algorithm when wrap-around
		if (next_hull_point_index == left_most_index)
		{
			m_count = out_count;
			break;
		}
	}

	//Copy vertices into shapes vertices
	for (int i = 0; i < m_count; i++)
		m_vertices[i] = vertices[hull_indexes[i]];

	for (int i = 0; i < m_count; i++)
	{
		int i2 = i + 1 < m_count ? i + 1 : 0;
		Vector2 face = m_vertices[i2] - m_vertices[i];

		// Calculate normal with 2D cross product between vector and scalar
		//m_normals[i] = Vector2(face.y, -face.x);
		m_normals[i] = -Cross(face, 1.0f);
		m_normals[i].Normalize();
	}
}

Vector2 Polygon::GetSupport(const Vector2& direction) const
{
	float best_projection = -FLT_MAX;
	Vector2 best_vertex(0.f, 0.f);

	for (int i = 0; i < m_count; i++)
	{
		Vector2 vertex = m_vertices[i];

		float projection = Dot(vertex, direction);

		if (projection > best_projection)
		{
			best_projection = projection;
			best_vertex = vertex;
		}
	}

	return best_vertex;
}