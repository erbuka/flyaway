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
	m_CameraPosition(0, 1, 0)
{

	m_VerticesCount = m_Width * m_Depth * 6;

	// Create first biome
	m_Biome = new GreenHills();
		
	// Let's create a triangle grid
	m_Vertices = new Vertex[m_Width * m_Depth * 6];

	for (int z = 0; z < m_Depth; z++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			float bx = (x - m_Width / 2) * m_TileSize;
			float bz = -z * m_TileSize;

			m_Vertices[z * m_Depth * 6 + x * 6 + 0].Position = Vector3f(bx, 0, bz);
			m_Vertices[z * m_Depth * 6 + x * 6 + 1].Position = Vector3f(bx, 0, bz + m_TileSize);
			m_Vertices[z * m_Depth * 6 + x * 6 + 2].Position = Vector3f(bx + m_TileSize, 0, bz);

			m_Vertices[z * m_Depth * 6 + x * 6 + 3].Position = Vector3f(bx + m_TileSize, 0, bz);
			m_Vertices[z * m_Depth * 6 + x * 6 + 4].Position = Vector3f(bx, 0, bz + m_TileSize);
			m_Vertices[z * m_Depth * 6 + x * 6 + 5].Position = Vector3f(bx + m_TileSize, 0, bz + m_TileSize);
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
	glFastFail(glBufferData(GL_ARRAY_BUFFER, m_Width * m_Depth * 6 * sizeof(Vertex), (void*)m_Vertices, GL_DYNAMIC_DRAW));
	glFastFail(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));			// Position
	glFastFail(glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)12));	// Normal
	glFastFail(glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)24));	// Diffusion Color
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, 0));

}

fa::Scene::~Scene()
{
	//delete[] m_Vertices;
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
	ComputeFaceNormals();

	// Transfer data to VBO
	glFastFail(glBindBuffer(GL_ARRAY_BUFFER, m_Vbo));
	glFastFail(glBufferData(GL_ARRAY_BUFFER, m_Width * m_Depth * 6 * sizeof(Vertex), (void*)m_Vertices, GL_DYNAMIC_DRAW));
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

	glDrawArrays(GL_TRIANGLES, 0, m_Width * m_Depth * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void fa::Scene::ComputeFaceNormals()
{
	// (Easy) face normal compute
	
	// Foreach triangle (CCW edges)...
	for (int i = 0; i < m_Width * m_Depth * 6; i += 3)
	{
		auto normal = Cross(m_Vertices[i + 2].Position - m_Vertices[i + 1].Position,
			m_Vertices[i + 1].Position - m_Vertices[i + 0].Position).Normalized();

		m_Vertices[i + 0].Normal = m_Vertices[i + 1].Normal = m_Vertices[i + 2].Normal = normal;
	}
}

void fa::Scene::UpdateTerrain(int fromVertex, int count)
{
	count = std::min(count + fromVertex, m_VerticesCount);
	for (int i = fromVertex; i < count; i++)
	{
		auto biomeDescr = m_Biome->GenerateAt(m_Vertices[i].Position + m_CameraPosition);
		m_Vertices[i].Position.Y = biomeDescr.TerrainHeight;
	}
}
