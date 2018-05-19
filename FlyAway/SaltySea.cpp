#include "SaltySea.h"
#include "SceneObject.h"
#include "Noise.h"

fa::SaltySea::SaltySea()
{
	m_TerrainPerlin = std::unique_ptr<Perlin>(new Perlin(4096.0f));
	m_TerrainPerlin->SetPersistance(0.8f);
}

fa::SaltySea::~SaltySea()
{
}

fa::BiomeTerrainDescriptor fa::SaltySea::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;
	result.TerrainHeight = -80 + m_TerrainPerlin->Sample({ x, z }) * 120;
	result.TerrainColor = 0xffffff;
	return result;
}

fa::SceneObject * fa::SaltySea::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	return nullptr;
}
