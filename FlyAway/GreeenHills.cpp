#include "GreeenHills.h"
#include "Noise.h"
#include "Util.h"
#include "Engine.h"
#include "Terrain.h"
#include "SceneObject.h"
#include <vector>

namespace _GreenHills
{
	fa::Interpolator<fa::Vector3f> TerrainColor = {
		{ 0.0f, fa::Vector3f::GetColor(166, 145, 123) },
		{ 0.5f, fa::Vector3f::GetColor(143, 204, 51) },
		{ 1.0f, fa::Vector3f::GetColor(188, 224, 133) }
	};
}


fa::GreenHills::GreenHills()
{
	m_TerrainHeight = std::unique_ptr<Perlin>(new Perlin(300));
	m_TerrainHeight->SetPersistance(0.3);

	m_ForestPerlin = std::unique_ptr<Perlin>(new Perlin(128));
	m_ForestPerlin->SetPersistance(0.6f);
	
	std::shared_ptr<ModelRandomGenerator> cypress(new ModelRandomGenerator{
		Engine::GetInstance()->GetModel(Models::Cypress0DarkGreen),
		Engine::GetInstance()->GetModel(Models::Cypress0LightGreen)
	});

	std::shared_ptr<ModelRandomGenerator> nothing(new ModelRandomGenerator{ nullptr });

	std::shared_ptr<ModelRandomGenerator> treeChance(new ModelRandomGenerator{ "tree", cypress, nothing });

	m_ModelGenerator = std::unique_ptr<ModelRandomGenerator>(new ModelRandomGenerator{ "forest", treeChance, nothing });
	
}

fa::GreenHills::~GreenHills()
{
}

fa::SceneObject * fa::GreenHills::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	
	Vector3f center = bounds.Center();
	float height = terrain->GetHeightAt(center);
	float scale = Random::NextValue(0.7f, 1.1f);

	float x = Random::NextFit(scale, bounds.Min.X, bounds.Max.X);
	float z = Random::NextFit(scale, bounds.Min.Z, bounds.Max.Z);

	std::map<std::string, float> inputs = { 
		{ "forest", m_ForestPerlin->Sample({ x, z}) > 0.5f ? 1.0f : 0.0f },
		{ "tree", 0.3f },
		{ "rock", 0.02f }
	};

	auto model = m_ModelGenerator->GetResult(inputs);

	return model == nullptr ? nullptr : new SceneObject(model, { x, height, z }, { scale, scale, scale });
}

fa::BiomeTerrainDescriptor fa::GreenHills::DescribeTerrainAtXY(float x, float z)
{

	BiomeTerrainDescriptor result;

	float sample = m_TerrainHeight->Sample({ x, z });

	fa::RandomTreeNode<fa::Vector3f>::Inputs inputs = {
		{ "height", sample }
	};
	
	result.TerrainHeight =  sample * 200;
	result.TerrainColor = _GreenHills::TerrainColor.Sample(m_ForestPerlin->Sample({ x, z }));

	return result;
}
