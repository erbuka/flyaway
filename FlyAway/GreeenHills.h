#pragma once

#include "Biome.h"

namespace fa
{

	class Perlin;

	class GreenHills : public Biome
	{
	public:
		GreenHills();
		~GreenHills();
		virtual BiomeDescriptor GenerateAtXZ(float x, float z) override;
	private:
		Perlin * m_Perlin;
		Perlin * m_ColorPerlin;
	};
}