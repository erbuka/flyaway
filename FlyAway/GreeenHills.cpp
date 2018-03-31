#include "GreeenHills.h"
#include "Noise.h"
#include "Util.h"
#include <vector>

fa::Interpolator<fa::Vector3f> _Colors = {
	// Grass
	{ 0.0f, fa::Vector3f::GetColor(69, 145, 45) },
	{ 0.2f, fa::Vector3f::GetColor(55, 111, 38) },
	{ 0.4f, fa::Vector3f::GetColor(49, 95, 33) },
	{ 0.7f, fa::Vector3f::GetColor(36, 72, 28) },

	// Dirt
	{ 1.0f, fa::Vector3f::GetColor(241, 196, 154) }
};

fa::GreenHills::GreenHills()
{
	m_Perlin = new Perlin(1024);
	m_Perlin->SetPersistance(0.6);

	m_ColorPerlin = new Perlin(128);
	m_ColorPerlin->SetPersistance(0.5f);
}

fa::GreenHills::~GreenHills()
{
	delete m_Perlin;
}

fa::BiomeDescriptor fa::GreenHills::GenerateAtXZ(float x, float z)
{

	BiomeDescriptor result;

	float sample = m_Perlin->Sample({ x, z });
	float colorSample = m_ColorPerlin->Sample({ x, z });
	
	result.TerrainHeight =  sample * 200;
	result.TerrainColor = _Colors.Sample(colorSample);

	return result;
}
