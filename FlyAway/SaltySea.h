#pragma once

#include "Biome.h"
#include "Model.h"

namespace fa
{
	class Perlin;

	class SaltySea : public Biome
	{
	public:
		SaltySea();
		~SaltySea();
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;
		virtual SceneObject* GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds) override;
	private:
		std::unique_ptr<Perlin> m_TerrainPerlin, m_BeachColor;
		std::unique_ptr<ModelRandomGenerator> m_ModelGenerator;
	};
}