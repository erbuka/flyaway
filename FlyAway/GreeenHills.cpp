#include "GreeenHills.h"
#include "Noise.h"
#include <iostream>
fa::GreenHills::GreenHills()
{
	m_Perlin = new Perlin(1024);
	m_Perlin->SetPersistance(0.60f);
}

fa::GreenHills::~GreenHills()
{
	delete m_Perlin;
}

fa::BiomeDescriptor fa::GreenHills::GenerateAt(float x, float z)
{
	BiomeDescriptor result;
	result.TerrainHeight = m_Perlin->Sample({ x, z }) * 200;
	return result;
}
