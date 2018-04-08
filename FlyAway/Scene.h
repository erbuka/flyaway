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

	private:

		std::shared_ptr<Biome> RandomBiome();

		BoundingBox3f GetNextChunkBounds();

		void UpdateBiome(float elapsedTime);
		void UpdateWorld(float elapsedTime);

		std::vector<Terrain*> m_Terrain;
		std::vector<Vector3f> m_CameraWaypoints;

		MatrixStack4f m_Projection, m_ModelView;
		int m_Width, m_ChunkDepth, m_WorldChunks;
		float m_TileSize, m_SightRange;
		Engine * m_Engine;
		Vector3f m_CameraPosition;
		BiomeInterpolator * m_BiomeInterpolator;

	};
}