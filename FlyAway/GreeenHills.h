#pragma once

#include "BiomeGenerator.h"

namespace fa
{

	class Perlin;

	class GreenHills : public BiomeGenerator
	{
	public:
		GreenHills();
		~GreenHills();

		virtual BiomeDescriptor GenerateAt(float x, float z) override;

	private:
		Perlin * m_Perlin;
	};
}