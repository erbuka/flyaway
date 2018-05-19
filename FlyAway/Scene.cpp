#include "Scene.h"
#include "Engine.h"
#include "GreeenHills.h"
#include "Desert.h"
#include "SnowyMountains.h"
#include "Terrain.h"
#include "SceneObject.h"
#include "Model.h"
#include <vector>
#include <future>

fa::Scene::Scene(Engine * engine, int width, int chunkDepth, float tileSize, float sightRange) :
	m_Engine(engine),
	m_Width(width),
	m_TileSize(tileSize),
	m_ChunkDepth(chunkDepth),
	m_SightRange(sightRange)
{
	m_WorldChunks = std::ceil(sightRange / (m_TileSize * m_ChunkDepth));
	m_BiomeInterpolator = new BiomeInterpolator();

	m_CurrentWorldChunk = {
		Vector3f(-m_Width / 2 * m_TileSize, 0, -m_ChunkDepth * m_TileSize),
		Vector3f(m_Width / 2 * m_TileSize, 0, 0)
	};

}

fa::Scene::~Scene()
{
	for (auto terrain : m_Terrain)
	{
		delete terrain;
	}

	delete m_BiomeInterpolator;
}

void fa::Scene::Update(float elapsedTime)
{

	// Update Camera Position
	/*
	if (m_CameraWaypoints.size() > 0)
	{
		bool hit;
		m_CameraPosition = m_CameraPosition.MoveTowards(m_CameraWaypoints[0], elapsedTime * 50, hit);
		if (hit)
		{
			m_CameraWaypoints.erase(m_CameraWaypoints.begin());
		}
	}
	*/

	// Update World
	UpdateWorld(elapsedTime);

	// Update camera
	if (m_Terrain.size() > 0)
	{
		float maxSteepness = -std::numeric_limits<float>::max();
		Vector3f target;
		bool hit;

		for (auto t = m_Terrain.begin() + 1; t != m_Terrain.end(); ++t)
		{
			Vector3f current = (*t)->GetBounds().Center();
			current = current + Vector3f(0, (*t)->GetHeightAt(current), 0);

			float steepness = (current - m_CameraPosition).Normalized() ^ Vector3f(0, 1, 0);

			if (steepness > maxSteepness)
			{
				maxSteepness = steepness;
				target = current;
			}

		}

		Vector3f desiredVelocity = (target + Vector3f(0, 5, 0) - m_CameraPosition).Normalized() * MaxCameraSpeed;
		Vector3f steering = (desiredVelocity - m_CameraVelocity);

		m_CameraVelocity = (m_CameraVelocity + steering * elapsedTime).Clamp(MaxCameraSpeed);

		m_CameraPosition = m_CameraPosition + m_CameraVelocity * elapsedTime;

	}

}

void fa::Scene::Render(GLuint program)
{

	float width = Engine::GetInstance()->GetWindowWidth(),
		height = Engine::GetInstance()->GetWindowHeight();

	m_Projection.LoadIdentity();
	m_Projection.Perspective(3.141592 / 4, width / height, 0.1, m_SightRange);
	//m_Projection.Orthographic(-20, 20, 300, 0, -1000, 1000);
	m_ModelView.LoadIdentity();
	m_ModelView.Multiply(Matrix4f::LookRotation(m_CameraVelocity.Normalized()));
	m_ModelView.Translate(-m_CameraPosition.X, -m_CameraPosition.Y, -m_CameraPosition.Z);
	

	GLint prLoc = glGetUniformLocation(program, "in_ProjectionMatrix");
	GLint mvLoc = glGetUniformLocation(program, "in_ModelViewMatrix");

	for (auto terrain : m_Terrain)
	{

		glUniformMatrix4fv(prLoc, 1, GL_TRUE, m_Projection.Current().Ptr());
		glUniformMatrix4fv(mvLoc, 1, GL_TRUE, m_ModelView.Current().Ptr());

		glBindVertexArray(terrain->GetVAO());
		glDrawElements(GL_TRIANGLES, terrain->GetIndicescount(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		auto& sceneObjGrid = terrain->GetSceneObjects();
		
		for (int x = 0; x < sceneObjGrid.GetCellsX(); x++)
		{
			for (int z = 0; z < sceneObjGrid.GetCellsZ(); z++)
			{
				auto sceneObj = sceneObjGrid.At(x, z).Object;
				if (sceneObj != nullptr) {
					m_ModelView.Push();
					m_ModelView.Multiply(sceneObj->GetTransform());

					glUniformMatrix4fv(prLoc, 1, GL_TRUE, m_Projection.Current().Ptr());
					glUniformMatrix4fv(mvLoc, 1, GL_TRUE, m_ModelView.Current().Ptr());

					sceneObj->GetModel()->Render();

					m_ModelView.Pop();
				}
			}
		}
	}
}


std::shared_ptr<fa::Biome> fa::Scene::RandomBiome()
{
	switch (Random::NextValue<int>(0, 3))
	{
		case 0: return std::shared_ptr<Biome>(new GreenHills());
		case 1: return std::shared_ptr<Biome>(new Desert());
		case 2: return std::shared_ptr<Biome>(new SnowyMountains());
		default: return std::shared_ptr<Biome>(nullptr);
	}
}

fa::BoundingBox3f fa::Scene::GetNextChunkBounds()
{
	/*
	static bool firstCall = true;
	static BoundingBox3f current;

	if (firstCall)
	{
		current = { 
			Vector3f(-m_Width / 2 * m_TileSize, 0, -m_ChunkDepth * m_TileSize),
			Vector3f(m_Width / 2 * m_TileSize, 0, 0) 
		};
		firstCall = false;
	}
	else
	{
		current.Min = current.Min + Vector3f(0, 0, -m_ChunkDepth * m_TileSize);
		current.Max = current.Max + Vector3f(0, 0, -m_ChunkDepth * m_TileSize);
	}
	*/

	m_CurrentWorldChunk.Min = m_CurrentWorldChunk.Min + Vector3f(0, 0, -m_ChunkDepth * m_TileSize);
	m_CurrentWorldChunk.Max = m_CurrentWorldChunk.Max + Vector3f(0, 0, -m_ChunkDepth * m_TileSize);


	return m_CurrentWorldChunk;
}

void fa::Scene::UpdateBiome(float elapsedTime)
{

}

void fa::Scene::UpdateWorld(float elapsedTime)
{
	// Remove OOB Terrain
	for (auto terrain = m_Terrain.begin(); terrain != m_Terrain.end();)
	{

		if ((*terrain)->GetBounds().Min.Z > m_CameraPosition.Z)
		{
			delete (*terrain);
			terrain = m_Terrain.erase(terrain);
		}
		else
		{
			++terrain;
		}
	}

	// Remove OOB Objects



	// GenerateTerrain new Biomes (TEMP)
	while (m_BiomeInterpolator->GetTransitionsCount() < 5)
	{
		m_BiomeInterpolator->PushTransition(RandomBiome(), Random::NextValue(-200, -300), Random::NextValue(-300, -600));
	}

	// GenerateTerrain new world chunks
	while (m_Terrain.size() < m_WorldChunks)
	{

		BoundingBox3f bounds = GetNextChunkBounds();
		Terrain * terrain = new Terrain(m_Width, m_ChunkDepth, bounds);

		m_BiomeInterpolator->GenerateTerrain(terrain);

		m_Terrain.push_back(terrain);
	}

	m_BiomeInterpolator->Cleanup(m_CurrentWorldChunk.Min.Z);

}
