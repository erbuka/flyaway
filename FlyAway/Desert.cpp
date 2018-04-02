#include "Desert.h"
#include "Noise.h"
#include "Engine.h"
#include "SceneObject.h"
#include "Terrain.h"

fa::Desert::Desert()
{
	m_Perlin = new Perlin(1024.0f);
	m_Perlin->SetPersistance(0.25f);
}

fa::BiomeTerrainDescriptor fa::Desert::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;

	result.TerrainHeight = m_Perlin->Sample({ x, z }) * 50.0f;
	result.TerrainColor = { 0.8f, 0.7f, 0.1f };

	return result;
}


fa::SceneObject * fa::Desert::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	Engine * engine = Engine::GetInstance();
	Model * model = engine->GetModel(Models::RedCube);
	auto position = bounds.Center();
	//return new SceneObject(model, position + Vector3f(0, terrain->GetHeightAt(position), 0));
	return nullptr;
}