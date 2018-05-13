#include "SnowyMountains.h"
#include "Engine.h"
#include "Terrain.h"
#include "SceneObject.h"

namespace _SnowyMountains
{
	fa::Interpolator<fa::Vector3f> TerrainColor = {
		{ 0.0f, fa::Vector3f::GetColor(99, 77, 56) },
		{ 0.48f, fa::Vector3f::GetColor(99, 77, 56) },
		{ 0.52f, fa::Vector3f::GetColor(255, 255, 255) },
		{ 1.0f, fa::Vector3f::GetColor(255, 255, 255) }
	};
}

fa::SnowyMountains::SnowyMountains()
{
	m_Height = std::unique_ptr<Perlin>(new Perlin(256.0f));
	m_Snow = std::unique_ptr<Perlin>(new Perlin(256.0f));

	m_Height->SetPersistance(0.25f);
	m_Snow->SetPersistance(0.9f);

	std::shared_ptr<ModelRandomGenerator> item(new ModelRandomGenerator{
		Engine::GetInstance()->GetModel(Models::XmasTree0DarkGreen),
		Engine::GetInstance()->GetModel(Models::Rock0DarkBrown)
	});

	std::shared_ptr<ModelRandomGenerator> nothing(new ModelRandomGenerator{ nullptr });

	m_ModelGenerator = std::unique_ptr<ModelRandomGenerator>(new ModelRandomGenerator{ "itemChance", item, nothing });

}

fa::BiomeTerrainDescriptor fa::SnowyMountains::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;
	result.TerrainColor = _SnowyMountains::TerrainColor.Sample(m_Snow->Sample({ x, z }));
	result.TerrainHeight = 40.0f + m_Height->Sample({ x, z }) * 300.0f;
	return result;
}

fa::SceneObject * fa::SnowyMountains::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
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
