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
		{ 0.4f, fa::Vector3f::GetColor(143, 204, 51) },
		{ 1.0f, fa::Vector3f::GetColor(188, 224, 133) }
	};
}


fa::GreenHills::GreenHills()
{
	m_Perlin = new Perlin(300);
	m_Perlin->SetPersistance(0.3);

	m_ColorPerlin = new Perlin(128);
	m_ColorPerlin->SetPersistance(0.5f);
	
	std::shared_ptr<ModelRandomGenerator> cypress(new ModelRandomGenerator{
		Engine::GetInstance()->GetModel(Models::DarkGreenCypress),
		Engine::GetInstance()->GetModel(Models::LightGreenCypress)
	});

	std::shared_ptr<ModelRandomGenerator> nothing(new ModelRandomGenerator{ nullptr });

	m_ModelGenerator = std::shared_ptr<ModelRandomGenerator>(new ModelRandomGenerator({ "object", cypress, nothing }));
	
}

fa::GreenHills::~GreenHills()
{
	delete m_Perlin;
	delete m_ColorPerlin;
}

fa::SceneObject * fa::GreenHills::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	/*
	Vector3f center = bounds.Center();
	float height = terrain->GetHeightAt(center);
	float scale = Random::NextValue(0.7f, 1.1f);

	float x = Random::NextFit(scale, bounds.Min.X, bounds.Max.X);
	float z = Random::NextFit(scale, bounds.Min.Z, bounds.Max.Z);

	std::map<std::string, float> inputs = { { "object", m_Perlin->Sample({ x, z}) } };

	auto model = m_ModelGenerator->GetResult(inputs);

	return model == nullptr ? nullptr : new SceneObject(model, { x, height, z }, { scale, scale, scale });
	
	*/
	return nullptr;
}

fa::BiomeTerrainDescriptor fa::GreenHills::DescribeTerrainAtXY(float x, float z)
{

	BiomeTerrainDescriptor result;

	float sample = m_Perlin->Sample({ x, z });
	//float colorSample = m_ColorPerlin->Sample({ x, z });

	fa::RandomTreeNode<fa::Vector3f>::Inputs inputs = {
		{ "height", sample }
	};
	
	result.TerrainHeight =  sample * 200;
	result.TerrainColor = _GreenHills::TerrainColor.Sample(sample);

	return result;
}
