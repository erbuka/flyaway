#include "Terrain.h"
#include "Util.h"

fa::Terrain::Terrain(int divisionsX, int divisionsZ, const BoundingBox3f& bounds) :
	m_VerticesX(divisionsX + 1),
	m_VerticesZ(divisionsZ + 1),
	m_Bounds(bounds),
	m_VAO(0),
	m_VB(0),
	m_IB(0)
{
	m_VerticesCount = m_VerticesX * m_VerticesZ;

	m_Vertices = new Vertexf[m_VerticesCount];
	m_Adjacency = new Vertexf*[4];
	m_Adjacency[Left] = new Vertexf[m_VerticesZ];
	m_Adjacency[Right] = new Vertexf[m_VerticesZ];
	m_Adjacency[Up] = new Vertexf[m_VerticesX];
	m_Adjacency[Down] = new Vertexf[m_VerticesX];

	m_TileWidth = bounds.Width() / divisionsX;
	m_TileDepth = bounds.Depth() / divisionsZ;

	// Create vertices
	for (int z = 0; z < m_VerticesZ; z++)
	{
		for (int x = 0; x < m_VerticesX; x++)
		{
			m_Vertices[z * m_VerticesX + x].Position = {
				bounds.Min.X + m_TileWidth * x,
				0,
				bounds.Min.Z + m_TileDepth * z
			};
		}
	}

	// Create indices for triangles
	for (int z = 0; z < m_VerticesZ - 1; z++)
	{
		for (int x = 0; x < m_VerticesX - 1; x++)
		{

			m_Indices.push_back(z * m_VerticesX + x);
			m_Indices.push_back((z + 1) * m_VerticesX + x);
			m_Indices.push_back(z * m_VerticesX + x + 1);

			m_Indices.push_back(z * m_VerticesX + x + 1);
			m_Indices.push_back((z + 1) * m_VerticesX + x);
			m_Indices.push_back((z + 1) * m_VerticesX + x + 1);
		}
	}

	// Create adjacency vertices
	for (int x = 0; x < m_VerticesX; x++)
	{
		m_Adjacency[Up][x].Position = { m_Bounds.Min.X + x * m_TileWidth, 0, m_Bounds.Min.Z - m_TileDepth };
		m_Adjacency[Down][x].Position = { m_Bounds.Min.X + x * m_TileWidth, 0, m_Bounds.Max.Z + m_TileDepth };
	}

	for (int z = 0; z < m_VerticesZ; z++)
	{
		m_Adjacency[Left][z].Position = { m_Bounds.Min.X - m_TileWidth, 0, m_Bounds.Min.Z + z * m_TileDepth };
		m_Adjacency[Right][z].Position = { m_Bounds.Max.X + m_TileWidth, 0, m_Bounds.Min.Z + z * m_TileDepth };
	}



}

fa::Terrain::~Terrain()
{
	glFastFail(glDeleteBuffers(1, &m_VB));
	glFastFail(glDeleteBuffers(1, &m_IB));
	glFastFail(glDeleteVertexArrays(1, &m_VAO));

	delete[] m_Vertices;

	for (int i = 0; i < 4; i++)
	{
		delete[] m_Adjacency[i];
	}

	delete[] m_Adjacency;

	for (auto sceneObj : m_SceneObjects)
	{
		delete sceneObj;
	}

}

int fa::Terrain::GetVerticesX() const
{
	return m_VerticesX;
}

int fa::Terrain::GetVerticesZ() const
{
	return m_VerticesZ;
}

int fa::Terrain::GetVerticesCount() const
{
	return m_VerticesCount;
}

int fa::Terrain::GetIndicescount() const
{
	return m_Indices.size();
}

void fa::Terrain::SetAdjacenty(EAdjacency adjacency, int index, const Vertexf & vertex)
{
	m_Adjacency[adjacency][index] = vertex;
}

fa::Vertexf & fa::Terrain::GetAdjacency(EAdjacency adjacency, int index)
{
	return m_Adjacency[adjacency][index];
}

void fa::Terrain::GenerateVertexArray()
{

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glBufferData(GL_ARRAY_BUFFER, m_VerticesCount * sizeof(Vertexf), (void*)m_Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexf), (void*)0);	// Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertexf), (void*)12);	// Normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertexf), (void*)24);	// Diffusion Color

	glGenBuffers(1, &m_IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), (void*)m_Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

}

void fa::Terrain::ComputeNormals()
{
	for (int z = 0; z < m_VerticesZ; z++)
	{
		for (int x = 0; x < m_VerticesX; x++)
		{
			Vertexf &self = m_Vertices[z * m_VerticesX + x];

			Vector3f left = x == 0 ? m_Adjacency[Left][z].Position - self.Position :
				m_Vertices[z * m_VerticesX + x - 1].Position - self.Position;

			Vector3f right = x == m_VerticesX - 1 ? m_Adjacency[Right][z].Position - self.Position :
				m_Vertices[z * m_VerticesX + x + 1].Position - self.Position;

			Vector3f down = z == m_VerticesZ - 1 ? m_Adjacency[Down][x].Position - self.Position :
				m_Vertices[(z + 1) * m_VerticesX + x].Position - self.Position;

			Vector3f up = z == 0 ? m_Adjacency[Up][x].Position - self.Position :
				m_Vertices[(z - 1) * m_VerticesX + x].Position - self.Position;

			self.Normal = (Cross(left, down).Normalized() + Cross(down, right).Normalized() +
				Cross(right, up).Normalized() + Cross(up, left).Normalized()).Normalized();

		}
	}
}

GLuint fa::Terrain::GetVAO()
{
	return m_VAO;
}


fa::Vertexf & fa::Terrain::operator[](int index)
{
	return m_Vertices[index];
}

fa::Vertexf & fa::Terrain::At(int index)
{
	return (*this)[index];
}

float fa::Terrain::GetHeightAt(const Vector3f & v) const
{

	
	int v0x = (int)((v.X - m_Bounds.Min.X) / m_TileWidth);
	int v0z = (int)((v.Z - m_Bounds.Min.Z) / m_TileDepth);
	int v1x = v0x + 1, v1z = v0z + 1;

	Vertexf& v00 = m_Vertices[v0z * m_VerticesX + v0x];
	Vertexf& v01 = m_Vertices[v0z * m_VerticesX + v1x];
	Vertexf& v10 = m_Vertices[v1z * m_VerticesX + v0x];
	Vertexf& v11 = m_Vertices[v1z * m_VerticesX + v1x];

	float tx = (v.X - v00.Position.X) / m_TileWidth;
	float tz = (v.Z - v00.Position.Z) / m_TileDepth;

	return Util::Lerp(
		Util::Lerp(v00.Position.Y, v01.Position.Y, tx),
		Util::Lerp(v10.Position.Y, v11.Position.Y, tx),
		tz
		);

}

std::vector<fa::SceneObject*>& fa::Terrain::GetSceneObjects()
{
	return m_SceneObjects;
}

fa::BoundingBox3f & fa::Terrain::GetBounds()
{
	return m_Bounds;
}
