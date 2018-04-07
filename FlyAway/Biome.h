#pragma once

#include "Vecmath.h"

namespace fa
{

	class Terrain;
	class SceneObject;

	struct BiomeTerrainDescriptor
	{
		float TerrainHeight;
		Vector3f TerrainColor;
	};

	class Biome
	{
	public:
		virtual void GenerateTerrain(Terrain * terrain);
		virtual SceneObject* GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds);
		virtual BiomeTerrainDescriptor DescribeTerrainAt(const Vector3f& position);
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) = 0;
	};

	class BiomeInterpolator : public Biome
	{
	public:
		BiomeInterpolator(Biome * currentBiome);
		
		bool IsStable() const;

		std::shared_ptr<Biome> GetCurrentBiome() const;
		std::shared_ptr<Biome> GetNextBiome() const;

		void PushBiome(std::shared_ptr<Biome> nextBiome);

		void StartInterpolation(float startZ, float endZ, int step);

		void EndInterpolation();

		int GetInterpolationValue() const;


		virtual SceneObject* GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds) override;
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;

	private:

		static constexpr int MaxInterpolationSteps = 1000;

		std::shared_ptr<Biome> m_CurrentBiome, m_NextBiome;
		int m_InterpolationValue, m_InterpolationStep;
		double m_StartZ, m_EndZ;
	};
}