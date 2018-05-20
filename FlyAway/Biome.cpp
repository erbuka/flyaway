#include "Biome.h"
#include "Util.h"
#include "Terrain.h"
#include "GreeenHills.h"
#include <cassert>

void fa::Biome::GenerateTerrain(Terrain * terrain)
{

	// GenerateTerrain Adjacencies
	for (int z = 0; z < terrain->GetVerticesZ(); z++)
	{
		auto& left = terrain->GetAdjacency(Terrain::Left, z);
		auto& right = terrain->GetAdjacency(Terrain::Right, z);

		left.Position.Y = DescribeTerrainAt(left.Position).TerrainHeight;
		right.Position.Y = DescribeTerrainAt(right.Position).TerrainHeight;

	}

	for (int x = 0; x < terrain->GetVerticesX(); x++)
	{
		auto& up = terrain->GetAdjacency(Terrain::Up, x);
		auto& down = terrain->GetAdjacency(Terrain::Down, x);

		up.Position.Y = DescribeTerrainAt(up.Position).TerrainHeight;
		down.Position.Y = DescribeTerrainAt(down.Position).TerrainHeight;
	}

	// GenerateTerrain Vertices
	for (int i = 0; i < terrain->GetVerticesCount(); i++)
	{
		auto& vertex = (*terrain)[i];
		auto biomeDescr = DescribeTerrainAt(vertex.Position);
		vertex.Position.Y = biomeDescr.TerrainHeight;
		vertex.DiffuseColor = biomeDescr.TerrainColor;
	}

	// Generate Normals
	terrain->ComputeNormals();
	
	// Generate Water
	terrain->GenerateWater();

	auto& sceneObjGrid = terrain->GetSceneObjects();

	for (int x = 0; x < sceneObjGrid.GetCellsX(); x++)
	{
		for (int z = 0; z < sceneObjGrid.GetCellsZ(); z++)
		{

			auto& cell = sceneObjGrid.At(x, z);
			if (terrain->GetHeightAt(cell.Bounds.Center()) >= 0.0f)
			{
				cell.Object = GenerateSceneObject(terrain, cell.Bounds);
			}
		}
	}

}

fa::SceneObject * fa::Biome::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	return nullptr;
}

fa::BiomeTerrainDescriptor fa::Biome::DescribeTerrainAt(const Vector3f& position)
{
	return DescribeTerrainAtXY(position.X, position.Z);
}

fa::BiomeInterpolator::BiomeInterpolator() :
	m_DefaultBiome(std::shared_ptr<Biome>(new GreenHills()))
{
}

fa::SceneObject * fa::BiomeInterpolator::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{

	float z = bounds.Center().Z;
	std::shared_ptr<Transition> transition = GetCurrentTransition(z);

	if (transition == nullptr)
	{
		return GetCurrentBiome(z)->GenerateSceneObject(terrain, bounds);
	}
	else
	{
		float t = -(z - transition->MaxZ) / (transition->MaxZ - transition->MinZ);
		return Random::NextValue<float>() < t ?
			transition->NextBiome->GenerateSceneObject(terrain, bounds) :
			GetCurrentBiome(z)->GenerateSceneObject(terrain, bounds);
	}
}

fa::BiomeTerrainDescriptor fa::BiomeInterpolator::DescribeTerrainAtXY(float x, float z)
{

	std::shared_ptr<Transition> transition = GetCurrentTransition(z);

	if (transition == nullptr)
	{
		return GetCurrentBiome(z)->DescribeTerrainAtXY(x, z);
	}
	else
	{

		BiomeTerrainDescriptor result;


		double t = Fade(-(z - transition->MaxZ) / (transition->MaxZ - transition->MinZ));

		auto currentDescr = GetCurrentBiome(z)->DescribeTerrainAtXY(x, z);
		auto nextDescr = transition->NextBiome->DescribeTerrainAtXY(x, z);

		result.TerrainHeight = Util::Lerp(currentDescr.TerrainHeight, nextDescr.TerrainHeight, t);
		result.TerrainColor = Util::Lerp(currentDescr.TerrainColor, nextDescr.TerrainColor, t);

		return result;

	}

}

float fa::BiomeInterpolator::Fade(float t) const
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

std::shared_ptr<fa::BiomeInterpolator::Transition> fa::BiomeInterpolator::GetCurrentTransition(float z)
{
	for (int i = 0; i < m_Transitions.size(); ++i)
	{
		if (m_Transitions[i]->MaxZ >= z && m_Transitions[i]->MinZ <= z)
		{
			return m_Transitions[i];
		}
	}
	return nullptr;
}

std::shared_ptr<fa::Biome> fa::BiomeInterpolator::GetCurrentBiome(float z) const
{
	for (int i = m_Transitions.size() - 1; i >= 0; --i)
	{
		if (m_Transitions[i]->MinZ >= z)
		{
			return m_Transitions[i]->NextBiome;
		}
	}
	return m_DefaultBiome;
}

void fa::BiomeInterpolator::PushTransition(std::shared_ptr<Biome> biome, float length, float offsetZ)
{
	std::shared_ptr<Transition> last(m_Transitions.size() == 0 ? nullptr : m_Transitions[m_Transitions.size() - 1]);

	std::shared_ptr<Transition> t(new Transition());

	t->NextBiome = biome;
	t->MaxZ = last == nullptr ? offsetZ : last->MinZ + offsetZ;
	t->MinZ = t->MaxZ + length;

	m_Transitions.push_back(t);
}

void fa::BiomeInterpolator::Cleanup(float z)
{
	int index = -1;
	for (int i = m_Transitions.size() - 1; i >= 0; --i)
	{
		if (m_Transitions[i]->MinZ >= z)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		m_Transitions.erase(m_Transitions.begin(), m_Transitions.begin() + index);
	}

}

size_t fa::BiomeInterpolator::GetTransitionsCount()
{
	return m_Transitions.size();
}


