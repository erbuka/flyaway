#include "Biome.h"
#include "Util.h"
#include "Terrain.h"
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
}

void fa::Biome::GenerateSceneObjects(Engine * engine, Terrain * terrain)
{

}

fa::BiomeTerrainDescriptor fa::Biome::DescribeTerrainAt(const Vector3f& position)
{
	return DescribeTerrainAtXY(position.X, position.Z);
}

fa::BiomeInterpolator::BiomeInterpolator(Biome * currentBiome) :
	m_CurrentBiome(currentBiome),
	m_NextBiome(nullptr),
	m_InterpolationValue(0.0f),
	m_InterpolationStep(0.0f)
{
}

float fa::BiomeInterpolator::GetInterpolationValue() const
{
	return m_InterpolationValue;
}

void fa::BiomeInterpolator::GenerateSceneObjects(Engine * engine, Terrain * terrain)
{
	m_CurrentBiome->GenerateSceneObjects(engine, terrain);
}

fa::BiomeTerrainDescriptor fa::BiomeInterpolator::DescribeTerrainAtXY(float x, float z)
{
	if (IsStable())
	{
		return m_CurrentBiome->DescribeTerrainAtXY(x, z);
	}
	else
	{
		BiomeTerrainDescriptor result;

		// All z's are negative and:
		// -> z <= m_StartZ
		// -> m_EndZ <= m_StartZ
		// So t should be in [0, 1)
		double t = (z - m_StartZ) / (m_EndZ - m_StartZ); 

		double tFrom = m_InterpolationValue;
		double tTo = std::min(1.0, m_InterpolationValue + m_InterpolationStep);

		auto currentDescr = m_CurrentBiome->DescribeTerrainAtXY(x, z);
		auto nextDescr = m_NextBiome->DescribeTerrainAtXY(x, z);

		result.TerrainHeight = Util::Lerp(currentDescr.TerrainHeight, nextDescr.TerrainHeight, Util::Lerp(tFrom, tTo, t));

		result.TerrainColor = Util::Lerp(currentDescr.TerrainColor, nextDescr.TerrainColor, Util::Lerp(tFrom, tTo, t));

		return result;
	}
}

bool fa::BiomeInterpolator::IsStable() const
{
	return m_NextBiome == nullptr;
}

fa::Biome * fa::BiomeInterpolator::GetCurrentBiome() const
{
	return m_CurrentBiome;
}

fa::Biome * fa::BiomeInterpolator::GetNextBiome() const
{
	return m_NextBiome;
}

void fa::BiomeInterpolator::PushBiome(Biome * nextBiome)
{
	m_NextBiome = nextBiome;
	m_InterpolationValue = 0.0f;
}

void fa::BiomeInterpolator::StartInterpolation(float startZ, float endZ, float step)
{
	m_InterpolationStep = step;
	m_StartZ = startZ;
	m_EndZ = endZ;
}

void fa::BiomeInterpolator::EndInterpolation()
{
	m_InterpolationValue = std::min(1.0, m_InterpolationValue + m_InterpolationStep);
	if (m_InterpolationValue == 1.0 && !IsStable())
	{
		m_CurrentBiome = m_NextBiome;
		PushBiome(nullptr);
	}
}
