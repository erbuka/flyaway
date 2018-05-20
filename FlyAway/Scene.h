#pragma once

#include "MatrixStack.h"
#include <vector>

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
	private:

		static constexpr float MaxCameraSpeed = 50.0f;

		std::shared_ptr<Biome> RandomBiome();

		BoundingBox3f GetNextChunkBounds();
		
		void UpdateMatrices();
		void UpdateBiome(float elapsedTime);
		void UpdateWorld(float elapsedTime);

		std::vector<Terrain*> m_Terrain;

		MatrixStack4f m_Projection, m_ModelView;
		int m_Width, m_ChunkDepth, m_WorldChunks;
		float m_TileSize, m_SightRange;
		Engine * m_Engine;
		Vector3f m_CameraPosition, m_CameraVelocity;
		BiomeInterpolator * m_BiomeInterpolator;
		BoundingBox3f m_CurrentWorldChunk;

	};
}