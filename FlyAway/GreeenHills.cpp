#include "GreeenHills.h"
#include "Noise.h"
#include "Util.h"
#include "Engine.h"
#include "Terrain.h"
#include "SceneObject.h"
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

void fa::GreenHills::GenerateSceneObjects(Engine * engine, Terrain * terrain)
{
	auto model = engine->GetModel(Models::Cube);
	auto bounds = terrain->GetBounds();
	auto &objects = terrain->GetSceneObjects();
	for (float x = bounds.Min.X; x < bounds.Max.X; x += 2.5f)
	{
		for (float z = bounds.Min.Z; z < bounds.Max.Z; z += 2.5f)
		{
			if (Random::NextValue<float>() > 0.8f)
			{
				objects.push_back(new SceneObject(model, Vector3f(x, terrain->GetHeightAt({x, 0, z}), z)));
			}
		}
	}

}

fa::BiomeTerrainDescriptor fa::GreenHills::DescribeTerrainAtXY(float x, float z)
{

	BiomeTerrainDescriptor result;

	float sample = m_Perlin->Sample({ x, z });
	float colorSample = m_ColorPerlin->Sample({ x, z });
	
	result.TerrainHeight =  sample * 200;
	result.TerrainColor = _Colors.Sample(colorSample);

	return result;
}
