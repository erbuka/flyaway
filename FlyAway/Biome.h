#pragma once

#include "Vecmath.h"

namespace fa
{

	struct BiomeDescriptor
	{
		float TerrainHeight;
		Vector3f TerrainColor;
	};

	class Biome
	{
	public:
		virtual BiomeDescriptor GenerateAt(const Vector3f& position);
		virtual BiomeDescriptor GenerateAtXZ(float x, float z) = 0;
	};

	class BiomeInterpolator : public Biome
	{
	public:
		BiomeInterpolator(Biome * currentBiome);
		
		bool IsStable() const;

		Biome * GetCurrentBiome() const;
		Biome * GetNextBiome() const;

		void PushBiome(Biome * nextBiome);

		void StartInterpolation(float startZ, float endZ, float step);

		void EndInterpolation();

		float GetInterpolationValue() const;

		virtual BiomeDescriptor GenerateAtXZ(float x, float z);

	private:
		Biome * m_CurrentBiome, *m_NextBiome;
		double m_InterpolationValue, m_InterpolationStep;
		double m_StartZ, m_EndZ;
	};
}