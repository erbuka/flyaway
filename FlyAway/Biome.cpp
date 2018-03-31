#include "Biome.h"
#include "Util.h"
#include <cassert>

fa::BiomeDescriptor fa::Biome::GenerateAt(const Vector3f& position)
{
	return GenerateAtXZ(position.X, position.Z);
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

fa::BiomeDescriptor fa::BiomeInterpolator::GenerateAtXZ(float x, float z)
{
	if (IsStable())
	{
		return m_CurrentBiome->GenerateAtXZ(x, z);
	}
	else
	{
		BiomeDescriptor result;

		// All z's are negative and:
		// -> z <= m_StartZ
		// -> m_EndZ <= m_StartZ
		// So t should be in [0, 1)
		double t = (z - m_StartZ) / (m_EndZ - m_StartZ); 

		double tFrom = m_InterpolationValue;
		double tTo = std::min(1.0, m_InterpolationValue + m_InterpolationStep);

		auto currentDescr = m_CurrentBiome->GenerateAtXZ(x, z);
		auto nextDescr = m_NextBiome->GenerateAtXZ(x, z);

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
