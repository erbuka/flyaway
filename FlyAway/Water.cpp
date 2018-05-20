#include "Water.h"
#include "Terrain.h"
#include <vector>
#include <array>


namespace _Water
{
	fa::Interpolator<fa::Vector3f> Water = {
		{ 0.00f, 0x74CCF4 },
		{ 0.25f, 0x5ABCD8 },
		{ 0.50f, 0x1CA3EC },
		{ 0.75f, 0x2389DA },
		{ 1.00f, 0x0F5E9C }
	};
}

fa::Water::Water() :
	m_Vao(0),
	m_Vb(0)
{
}

fa::Water::~Water()
{
	glDeleteBuffers(1, &m_Vb);
	glDeleteVertexArrays(1, &m_Vao);
}

void fa::Water::Generate(const Terrain& terrain)
{

	/* Returns true if at least one vertex of the given triangle is below 0 Y */
	static auto isBelowZero = [](const std::array<Vector3f, 4> pts, int i0, int i1, int i2) -> bool
	{
		return pts[i0].Y < 0 || pts[i1].Y < 0 || pts[i2].Y < 0;
	};

	/* Returns the water color based on the height of the terrain */
	static auto getColor = [](float height) -> Vector3f
	{
		float depth = Util::Clamp(height, -5.0f, 0.0f) / -5.0f;
		return _Water::Water.Sample(depth);
	};

	/* 
	Go through the terrain (assuming that the height of each vertex has already
	been assigned), and generate a triangle for each triangle of the terrain
	which is below 0
	*/
	for (int z = 0; z < terrain.GetVerticesZ() - 1; z++)
	{
		for (int x = 0; x < terrain.GetVerticesX() - 1; x++)
		{
			std::array<Vector3f, 4> pts = {
				terrain[z * terrain.GetVerticesX() + x].Position,
				terrain[(z + 1) * terrain.GetVerticesX() + x].Position,
				terrain[(z + 1) * terrain.GetVerticesX() + x + 1].Position,
				terrain[z * terrain.GetVerticesX() + x + 1].Position
			};
			
			if (isBelowZero(pts, 0, 1, 2))
			{
				m_Vertices.push_back({ pts[0], Vector3f(0, 1, 0), getColor(pts[0].Y), pts[0] });
				m_Vertices.push_back({ pts[1], Vector3f(0, 1, 0), getColor(pts[1].Y), pts[1] });
				m_Vertices.push_back({ pts[2], Vector3f(0, 1, 0), getColor(pts[2].Y), pts[2] });
			}

			if (isBelowZero(pts, 2, 3, 0))
			{
				m_Vertices.push_back({ pts[2], Vector3f(0, 1, 0), getColor(pts[2].Y), pts[2] });
				m_Vertices.push_back({ pts[3], Vector3f(0, 1, 0), getColor(pts[3].Y), pts[3] });
				m_Vertices.push_back({ pts[0], Vector3f(0, 1, 0), getColor(pts[0].Y), pts[0] });
			}
		}
	}

	/* Set the height of the generate triangles to 0 (Water is at 0 Y by convention) */
	std::for_each(m_Vertices.begin(), m_Vertices.end(), [](Vertexf& v) { v.Position.Y = 0; });
	
	/* Generate buffers */
	
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vb);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vb);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertexf) * m_Vertices.size(), (void*)m_Vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertexf), (void*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertexf), (void*)24);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)36);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

}

int fa::Water::GetVerticesCount() const
{
	return m_Vertices.size();
}

GLuint fa::Water::GetVAO() const
{
	return m_Vao;
}
