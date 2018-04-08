#include "Desert.h"
#include "Engine.h"
#include "SceneObject.h"
#include "Terrain.h"

namespace _Desert
{
	fa::Interpolator<fa::Vector3f> SandColors = {
		{ 0.0f, fa::Vector3f::GetColor(255, 232, 196) },
		{ 1.0f, fa::Vector3f::GetColor(255, 221, 117) }
	};
}
fa::Desert::Desert()
{
	m_TerrainHeight = std::unique_ptr<Perlin>(new Perlin(1024.0f));
	m_TerrainHeight->SetPersistance(1.0f);

	m_SandColor = std::unique_ptr<Perlin>(new Perlin(16.0f));
	m_SandColor->SetPersistance(0.7f);

	std::shared_ptr<ModelRandomGenerator> item(new ModelRandomGenerator{
		Engine::GetInstance()->GetModel(Models::Cactus0PaleGreen),
		Engine::GetInstance()->GetModel(Models::Rock0LightBrown)
	});

	std::shared_ptr<ModelRandomGenerator> nothing(new ModelRandomGenerator{ nullptr });

	m_ModelGenerator = std::unique_ptr<ModelRandomGenerator>(new ModelRandomGenerator{ "itemChance", item, nothing });
}

fa::BiomeTerrainDescriptor fa::Desert::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;

	result.TerrainHeight = 20.0f + m_TerrainHeight->Sample({ x, z }) * 20.0f;
	result.TerrainColor = _Desert::SandColors.Sample(m_SandColor->Sample({ x, z }));

	return result;
}


fa::SceneObject * fa::Desert::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	float x = Random::NextFit(1.0f, bounds.Min.X, bounds.Max.X);
	float z = Random::NextFit(1.0f, bounds.Min.Z, bounds.Max.Z);

	auto position = Vector3f(x, terrain->GetHeightAt({ x, 0, z}), z);
	Vector4f rotation = { 0.0f, 1.0f, 0.0f, PI<float>() * 2.0f * Random::NextValue<float>() };

	ModelRandomGenerator::Inputs inputs = {
		{ "itemChance", 0.005f }
	};

	auto model = m_ModelGenerator->GetResult(inputs);

	return model == nullptr ? nullptr : new SceneObject(model, position, rotation);
}