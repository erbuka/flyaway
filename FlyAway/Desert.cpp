#include "Desert.h"
#include "Noise.h"

fa::Desert::Desert()
{
	m_Perlin = new Perlin(1024.0f);
	m_Perlin->SetPersistance(0.25f);
}

fa::BiomeDescriptor fa::Desert::GenerateAtXZ(float x, float z)
{
	BiomeDescriptor result;

	result.TerrainHeight = m_Perlin->Sample({ x, z }) * 50.0f;
	result.TerrainColor = { 0.8f, 0.7f, 0.1f };

	return result;
}
