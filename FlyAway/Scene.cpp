#include "Scene.h"
#include "Engine.h"
#include "GreeenHills.h"
#include <vector>
#include <future>

fa::Scene::Scene(Engine * engine, int width, int depth, float tileSize) :
	m_Engine(engine),
	m_Width(width),
	m_Depth(depth),
	m_TileSize(tileSize),
	m_CameraPosition(0, 1, 0),
	m_Indices()
{

	m_VerticesCount = m_Width * m_Depth;

	// Create first biome
	m_Biome = new GreenHills();
		
	// Create vertices
	m_Vertices = new Vertex[m_VerticesCount];


	for (int z = 0; z < m_Depth; z++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			m_Vertices[z * m_Width + x].Position = {
				(x - m_Width / 2) * m_TileSize,
				0,
				-z * m_TileSize
			};
		}
	}

	// Create indices for triangles
	for (int z = 0; z < m_Depth - 1; z++)
	{
		for (int x = 0; x < m_Width - 1; x++)
		{
			m_Indices.push_back(z * m_Width + x);
			m_Indices.push_back(z * m_Width + x + 1);
			m_Indices.push_back((z + 1) * m_Width + x);

			m_Indices.push_back((z + 1) * m_Width + x);
			m_Indices.push_back(z * m_Width + x + 1);
			m_Indices.push_back((z + 1) * m_Width + x + 1);
		}
	}


	// Initialize random color for testing
	for (int i = 0; i < m_VerticesCount; i++)
	{
		m_Vertices[i].DiffuseColor = {0.2f, 0.8f, 0.3f};
	}


	// Create a vertex buffer and fill with the data
	glFastFail(glGenBuffers(1, &m_Vbo));
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, m_Vbo));
	glFastFail(glBufferData(GL_ARRAY_BUFFER, m_VerticesCount * sizeof(Vertex), (void*)m_Vertices, GL_DYNAMIC_DRAW));
	glFastFail(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));			// Position
	glFastFail(glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)12));	// Normal
	glFastFail(glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)24));	// Diffusion Color
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, 0));

}

fa::Scene::~Scene()
{
	delete[] m_Vertices;
	glDeleteBuffers(1, &m_Vbo);
}

void fa::Scene::Update(float elapsedTime)
{
	constexpr int numThreads = 8;
	std::future<void> futures[numThreads];
	int verticesPerThread = std::ceil((float)m_VerticesCount / numThreads);

	// Update Camera Position
	auto biomeDescr = m_Biome->GenerateAt(m_CameraPosition);
	m_CameraPosition.Z -= 50.0f * elapsedTime;
	m_CameraPosition.Y = biomeDescr.TerrainHeight + 3;


	// Start updating threads...
	for (int i = 0; i < numThreads; i++)
	{
		futures[i] = std::async(&Scene::UpdateTerrain, this, i * verticesPerThread, verticesPerThread);
	}

	// Wait for threads to finish
	for (int i = 0; i < numThreads; i++)
	{
		futures[i].wait();
	}

	// Compute Normals (for now face normals)
	ComputeNormals();

	// Transfer data to VBO
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, m_Vbo));
	glFastFail(glBufferData(GL_ARRAY_BUFFER, m_VerticesCount * sizeof(Vertex), (void*)m_Vertices, GL_DYNAMIC_DRAW));
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, 0));

}

void fa::Scene::Render()
{

	m_Projection.LoadIdentity();
	m_Projection.Perspective(3.141592 / 4, fa::CWidth / (float)fa::CHeight, 0.1, 1000);
	
	m_ModelView.LoadIdentity();
	m_ModelView.Translate(-m_CameraPosition.X, -m_CameraPosition.Y, 0);

	auto mvMat = m_ModelView.Current();
	auto prMat = m_Projection.Current();

	auto basicProgram = m_Engine->GetProgram("basic");

	glUseProgram(basicProgram);

	glUniformMatrix4fv(glGetUniformLocation(basicProgram, "in_ProjectionMatrix"), 1, GL_TRUE, prMat.Ptr());
	glUniformMatrix4fv(glGetUniformLocation(basicProgram, "in_ModelViewMatrix"), 1, GL_TRUE, mvMat.Ptr());
	
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, m_Indices.data());

	// glDrawArrays(GL_TRIANGLES, 0, m_Width * m_Depth * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void fa::Scene::ComputeNormals()
{

	for (int z = 0; z < m_Depth; z++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			Vertex &self = m_Vertices[z * m_Width + x];

			Vector3f left = x == 0 ? Vector3f{ -1.0f, 0.0f, 0.0f } :
				m_Vertices[z * m_Width + x - 1].Position - self.Position;

			Vector3f right = x == m_Width - 1 ? Vector3f{ 1.0f, 0.0f, 0.0f } :
				m_Vertices[z * m_Width + x + 1].Position - self.Position;

			Vector3f down = z == 0 ? Vector3f{ 0.0f, 0.0f, 1.0f } :
				m_Vertices[(z - 1) * m_Width + x].Position - self.Position;

			Vector3f up = z == m_Depth - 1 ? Vector3f{ 0.0f, 0.0f, -1.0f } :
				m_Vertices[(z + 1) * m_Width + x].Position - self.Position;


			self.Normal = (Cross(left, down) + Cross(down, right) + Cross(right, up) + Cross(up, left)).Normalized();
			
		}
	}
}

void fa::Scene::UpdateTerrain(int fromVertex, int count)
{
	count = std::min(count + fromVertex, m_VerticesCount);
	for (int i = fromVertex; i < count; i++)
	{
		auto biomeDescr = m_Biome->GenerateAt(m_Vertices[i].Position + m_CameraPosition);
		//m_Vertices[i].Position.Y = 0;
		m_Vertices[i].Position.Y = biomeDescr.TerrainHeight;
	}
}
