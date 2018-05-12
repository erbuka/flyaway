#include "SnowyMountains.h"

namespace _SnowyMountains
{
	fa::Interpolator<fa::Vector3f> TerrainColor = {
		{ 0.0f, fa::Vector3f::GetColor(99, 77, 56) },
		{ 0.48f, fa::Vector3f::GetColor(99, 77, 56) },
		{ 0.52f, fa::Vector3f::GetColor(255, 255, 255) },
		{ 1.0f, fa::Vector3f::GetColor(255, 255, 255) }
	};
}

fa::SnowyMountains::SnowyMountains()
{
	m_Height = std::unique_ptr<Perlin>(new Perlin(256.0f));
	m_Snow = std::unique_ptr<Perlin>(new Perlin(256.0f));

	m_Height->SetPersistance(0.5f);
	m_Snow->SetPersistance(0.9f);
}

fa::BiomeTerrainDescriptor fa::SnowyMountains::DescribeTerrainAtXY(float x, float z)
{
	BiomeTerrainDescriptor result;
	result.TerrainColor = _SnowyMountains::TerrainColor.Sample(m_Snow->Sample({ x, z }));
	result.TerrainHeight = 40.0f + m_Height->Sample({ x, z }) * 150.0f;
	return result;
}
