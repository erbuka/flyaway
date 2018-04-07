#pragma once

#include "Biome.h"
#include "Noise.h"
#include "Model.h"

namespace fa
{

	class Desert : public Biome
	{
	public:
		Desert();
		virtual SceneObject * GenerateSceneObject(Terrain* terrain, BoundingBox3f bounds) override;
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;
	private:
		std::unique_ptr<Perlin> m_TerrainHeight, m_SandColor;
		std::unique_ptr<ModelRandomGenerator> m_ModelGenerator;
	};
}