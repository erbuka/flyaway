#include "BiomeGenerator.h"


fa::BiomeDescriptor fa::BiomeGenerator::GenerateAt(const Vector3f& position)
{
	return GenerateAt(position.X, position.Z);
}