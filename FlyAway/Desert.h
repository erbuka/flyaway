#pragma once

#include "Biome.h"

namespace fa
{
	class Perlin;

	class Desert : public Biome
	{
	public:
		Desert();
		virtual BiomeDescriptor GenerateAtXZ(float x, float z) override;
	private:
		Perlin * m_Perlin;
	};
}