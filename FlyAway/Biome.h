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

		struct Transition
		{
			std::shared_ptr<Biome> NextBiome;
			float MaxZ, MinZ;
		};

		BiomeInterpolator();

		void PushTransition(std::shared_ptr<Biome>, float length, float offsetZ = 0);
		void Cleanup(float z);
		size_t GetTransitionsCount();

		virtual SceneObject* GenerateSceneObject(Terrain * terrain, BoundingBox3f bounds) override;
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;

	private:


		std::shared_ptr<Biome> GetCurrentBiome(float z) const;
		std::shared_ptr<Transition> GetCurrentTransition(float z);
		float Fade(float t) const;

		std::vector<std::shared_ptr<Transition>> m_Transitions;

		std::shared_ptr<Biome> m_DefaultBiome;
	};
}