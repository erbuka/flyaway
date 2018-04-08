#pragma once


#include "Biome.h"
#include "Noise.h"

namespace fa
{
	class SnowyMountains : public Biome
	{
	public:
		SnowyMountains();
		virtual BiomeTerrainDescriptor DescribeTerrainAtXY(float x, float z) override;
	private:
		std::unique_ptr<Perlin> m_Height;
	};
}