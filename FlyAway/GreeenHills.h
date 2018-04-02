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
		Perlin * m_Perlin;
		Perlin * m_ColorPerlin;
		std::shared_ptr<ModelRandomGenerator> m_ModelGenerator;
	};
}