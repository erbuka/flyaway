#pragma once

#include "Vecmath.h"

namespace fa
{

	struct BiomeDescriptor
	{
		float TerrainHeight;
	};

	class BiomeGenerator
	{
	public:
		BiomeDescriptor GenerateAt(const Vector3f& position);
		virtual BiomeDescriptor GenerateAt(float x, float z) = 0;
	};
}