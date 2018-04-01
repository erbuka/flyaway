#include "Scene.h"
#include "Engine.h"
#include "GreeenHills.h"
#include "Desert.h"
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
	m_BiomeInterpolator = new BiomeInterpolator(new GreenHills());
	m_BiomeInterpolator->PushBiome(new GreenHills());
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
	if (m_CameraWaypoints.size() > 0)
	{
		bool hit;
		m_CameraPosition = m_CameraPosition.MoveTowards(m_CameraWaypoints[0], elapsedTime * 50, hit);
		if (hit)
		{
			m_CameraWaypoints.erase(m_CameraWaypoints.begin());
		}
	}

	// Update World
	UpdateWorld(elapsedTime);

}

void fa::Scene::Render()
{

	m_Projection.LoadIdentity();
	m_Projection.Perspective(3.141592 / 4, fa::CWidth / (float)fa::CHeight, 0.1, 1000);
	
	m_ModelView.LoadIdentity();
	m_ModelView.Translate(-m_CameraPosition.X, -m_CameraPosition.Y, -m_CameraPosition.Z);

	auto basicProgram = m_Engine->GetProgram("basic");

	GLint prLoc = glGetUniformLocation(basicProgram, "in_ProjectionMatrix");
	GLint mvLoc = glGetUniformLocation(basicProgram, "in_ModelViewMatrix");

	glUseProgram(basicProgram);

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

	glUseProgram(0);
}


fa::BoundingBox3f fa::Scene::GetNextChunkBounds()
{
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

	return current;
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

	if (m_BiomeInterpolator->IsStable())
	{
		m_BiomeInterpolator->PushBiome(Random::NextValue<float>() > 0.5f ? (Biome*)new GreenHills() : (Biome*)new Desert());
	}


	// GenerateTerrain new world chunks
	while (m_Terrain.size() < m_WorldChunks)
	{

		BoundingBox3f bounds = GetNextChunkBounds();
		Terrain * terrain = new Terrain(m_Width, m_ChunkDepth, bounds);

		m_BiomeInterpolator->StartInterpolation(bounds.Max.Z, bounds.Min.Z, 0.01f);
		m_BiomeInterpolator->GenerateTerrain(terrain);

		m_Terrain.push_back(terrain);

		// Insert camera waypoint
		auto biomeDescr = m_BiomeInterpolator->DescribeTerrainAt(bounds.Center());
		m_CameraWaypoints.push_back(bounds.Center() + Vector3f{0, 3 + biomeDescr.TerrainHeight, 0});

		m_BiomeInterpolator->EndInterpolation();
	}



}
