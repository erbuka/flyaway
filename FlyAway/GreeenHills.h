#pragma once

#include "Biome.h"
#include "Model.h"

namespace fa
{

	class Perlin;

	class GreenHills : public Biome
	{
	public:
		GreenHills();
		~GreenHills();
		virtual SceneObject * GenerateSceneObject(Terrain* terrain, BoundingBox3f bounds) override;
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;
	private:
		std::unique_ptr<Perlin> m_TerrainHeight, m_ForestPerlin;
		std::unique_ptr<ModelRandomGenerator> m_ModelGenerator;
	};
}