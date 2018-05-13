#pragma once

#include "Model.h"
#include "Biome.h"
#include "Noise.h"

namespace fa
{
	class SnowyMountains : public Biome
	{
	public:
		SnowyMountains();
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;
		virtual SceneObject* GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds) override;
	private:
		std::unique_ptr<Perlin> m_Height;
		std::unique_ptr<Perlin> m_Snow;

		std::shared_ptr<ModelRandomGenerator> m_ModelGenerator;
	};
}