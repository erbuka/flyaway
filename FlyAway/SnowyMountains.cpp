#include "SnowyMountains.h"

fa::SnowyMountains::SnowyMountains()
{
	m_Height = std::unique_ptr<Perlin>(new Perlin(256.0f));
	m_Height->SetPersistance(0.5f);
}

fa::BiomeTerrainDescriptor fa::SnowyMountains::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;
	result.TerrainColor = Vector3f::GetColor(255, 0, 0);
	result.TerrainHeight = 40.0f + m_Height->Sample({ x, z }) * 150.0f;
	return result;
}
