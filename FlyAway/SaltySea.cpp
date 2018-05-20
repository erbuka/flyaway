#include "SaltySea.h"
#include "SceneObject.h"
#include "Noise.h"
#include "Engine.h"
#include "Terrain.h"

namespace _SaltySea
{
	fa::Interpolator<fa::Vector3f> BeachColors = {
		{ 0.0f, 0xD8CEA3 },
		{ 1.0f, 0xEADBAE }
	};

	fa::Interpolator<fa::Vector3f> GrassColors = {
		{ 0.0f, 0xBCE085 },
		{ 1.0f, 0x8FCC33 }
	};

}
fa::SaltySea::SaltySea()
{
	m_TerrainPerlin = std::unique_ptr<Perlin>(new Perlin(1024.0f));
	m_TerrainPerlin->SetPersistance(0.8f);

	m_BeachColor = std::unique_ptr<Perlin>(new Perlin(16.0f));
	m_BeachColor->SetPersistance(0.7f);

	ModelRandomGenerator::Pointer nothing(new ModelRandomGenerator({ nullptr }));
	ModelRandomGenerator::Pointer palmTree(new ModelRandomGenerator({
		Engine::GetInstance()->GetModel(Models::PalmTree0LightGreen)
	}));
		
	m_ModelGenerator = std::unique_ptr<ModelRandomGenerator>(new ModelRandomGenerator("itemChance", palmTree, nothing));

}

fa::SaltySea::~SaltySea()
{
}

fa::BiomeTerrainDescriptor fa::SaltySea::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;
	result.TerrainHeight = -20 + m_TerrainPerlin->Sample({ x, z }) * 40;

	float colorSample = m_BeachColor->Sample({ x, z });
	float beachFactor = Util::Clamp(result.TerrainHeight, 0.0f, 1.0f);

	result.TerrainColor = Util::Lerp(_SaltySea::BeachColors.Sample(colorSample), _SaltySea::GrassColors.Sample(colorSample), beachFactor);
	return result;
}

fa::SceneObject * fa::SaltySea::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	float x = Random::NextFit(1.0f, bounds.Min.X, bounds.Max.X);
	float z = Random::NextFit(1.0f, bounds.Min.Z, bounds.Max.Z);

	auto position = Vector3f(x, terrain->GetHeightAt({ x, 0, z }), z);
	Vector4f rotation = { 0.0f, 1.0f, 0.0f, PI<float>() * 2.0f * Random::NextValue<float>() };

	ModelRandomGenerator::Inputs inputs = {
		{ "itemChance", 0.01f }
	};

	auto model = m_ModelGenerator->GetResult(inputs);

	return model == nullptr ? nullptr : new SceneObject(model, position, rotation);
}
