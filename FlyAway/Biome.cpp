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

	// Generate Normals
	
	/*
	for (int i = 0; i < terrain->GetVerticesCount(); i++)
	{
		auto& vertex = (*terrain)[i];
		auto position = vertex.Position;
			
		Vector3f down =	Vector3f(position.X, DescribeTerrainAt(vertex.Position + Vector3f(0, 0, 1)).TerrainHeight, position.Z + 1) - position;
		Vector3f up = Vector3f(position.X, DescribeTerrainAt(vertex.Position + Vector3f(0, 0, -1)).TerrainHeight, position.Z - 1) - position;
		Vector3f left =	Vector3f(position.X - 1, DescribeTerrainAt(vertex.Position + Vector3f(-1, 0, 0)).TerrainHeight, position.Z) - position;
		Vector3f right = Vector3f(position.X + 1, DescribeTerrainAt(vertex.Position + Vector3f(1, 0, 0)).TerrainHeight, position.Z) - position;

		vertex.Normal = (Cross(down, right) + Cross(right, up) + Cross(up, left) + Cross(left, down)).Normalized();
	}
	*/
	
	terrain->ComputeNormals();

	terrain->GenerateVertexArray();

	auto& sceneObjGrid = terrain->GetSceneObjects();

	for (int x = 0; x < sceneObjGrid.GetCellsX(); x++)
	{
		for (int z = 0; z < sceneObjGrid.GetCellsZ(); z++)
		{
			auto& cell = sceneObjGrid.At(x, z);
			cell.Object = GenerateSceneObject(terrain, cell.Bounds);
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

fa::BiomeInterpolator::BiomeInterpolator(Biome * currentBiome) :
	m_CurrentBiome(currentBiome),
	m_NextBiome(nullptr),
	m_InterpolationValue(0.0f),
	m_InterpolationStep(0.0f)
{
}

int fa::BiomeInterpolator::GetInterpolationValue() const
{
	return m_InterpolationValue;
}

fa::SceneObject * fa::BiomeInterpolator::GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds)
{
	if (IsStable())
	{
		return m_CurrentBiome->GenerateSceneObject(terrain, bounds);
	}
	else
	{
		return Random::NextValue<float>() < m_InterpolationValue / (double)MaxInterpolationSteps ?
			m_NextBiome->GenerateSceneObject(terrain, bounds) :
			m_CurrentBiome->GenerateSceneObject(terrain, bounds);
	}
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

		double tFrom = m_InterpolationValue / (double)MaxInterpolationSteps;
		double tTo = std::min(1.0, (m_InterpolationValue + m_InterpolationStep) / (double)MaxInterpolationSteps );

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

std::shared_ptr<fa::Biome> fa::BiomeInterpolator::GetCurrentBiome() const
{
	return m_CurrentBiome;
}

std::shared_ptr<fa::Biome> fa::BiomeInterpolator::GetNextBiome() const
{
	return m_NextBiome;
}

void fa::BiomeInterpolator::PushBiome(std::shared_ptr<Biome> nextBiome)
{
	m_NextBiome = nextBiome;
	m_InterpolationValue = 0;
}

void fa::BiomeInterpolator::StartInterpolation(float startZ, float endZ, int step)
{
	m_InterpolationStep = step;
	m_StartZ = startZ;
	m_EndZ = endZ;
}

void fa::BiomeInterpolator::EndInterpolation()
{
	m_InterpolationValue = std::min(MaxInterpolationSteps, m_InterpolationValue + m_InterpolationStep);

	if (m_InterpolationValue == MaxInterpolationSteps && !IsStable())
	{
		m_CurrentBiome = m_NextBiome;
		PushBiome(nullptr);
	}
}
