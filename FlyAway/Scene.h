#pragma once

#include "MatrixStack.h"
#include <vector>
#include <mutex>
#include <thread>

namespace fa 
{

	class Engine;
	class BiomeInterpolator;
	class Biome;
	class Terrain;


	class Scene
	{
	public:
		Scene(Engine * engine, int width, int chunkDepth, float tileSize, float sightRange);
		~Scene();

		void Update(float elapsedTime);
		void Render(GLuint program);
		void RenderWater(GLuint program);

		Matrix4f GetModelViewMatrix();

	private:

		static constexpr float MaxCameraSpeed = 50.0f;

		std::shared_ptr<Biome> RandomBiome();

		BoundingBox3f GetNextChunkBounds();
		std::shared_ptr<Terrain> GenerateNextWorldChunk();
		
		void InitializeWorld();
		void StartWorldGeneratorThread();
		void UpdateMatrices();
		void UpdateWorld(float elapsedTime);

		std::vector<std::shared_ptr<Terrain>> m_Terrain;
		std::shared_ptr<Terrain> m_NewTerrain;

		MatrixStack4f m_Projection, m_ModelView;
		int m_Width, m_ChunkDepth, m_WorldChunks;
		float m_TileSize, m_SightRange;
		Engine * m_Engine;
		Vector3f m_CameraPosition, m_CameraVelocity;
		std::shared_ptr<BiomeInterpolator> m_BiomeInterpolator;
		BoundingBox3f m_CurrentWorldChunk;

		std::thread m_WorldGenerator;
		std::mutex m_WorldGeneratorMutex;

	};
}