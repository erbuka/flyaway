#include "Noise.h"
#include "Util.h"
#include <algorithm>
#include <random>
#include <chrono>

float fa::Noise::Sample(const Vector3f & point)
{
	return SampleNormalized({
		Normalize(point.X),
		Normalize(point.Y),
		Normalize(point.Z)
	});

}

float fa::Noise::Sample(const Vector2f & point)
{
	return SampleNormalized({ Normalize(point.X), Normalize(point.Y) });
}

float fa::Noise::Normalize(float x)
{
	x = std::fmodf(x, m_DomainSize) / m_DomainSize;
	return x < 0 ? 1.0 + x : x;
}

fa::Perlin::Perlin(float domainSize) :
	Noise::Noise(domainSize)
{
	for (int i = 0; i < SeedSize; i++)
	{
		m_Seed[i] = i;
	}
	NewSeed();
}

fa::Perlin::~Perlin()
{
}

void fa::Perlin::NewSeed()
{
	std::shuffle(m_Seed.begin(), m_Seed.end(), 
		std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count()));
}

float fa::Perlin::SampleNormalized(const Vector3f & position)
{
	float amplitude = 1.0f;
	float amplitudeAcc = 0.0f;
	float result = 0.0f;
	unsigned int frequency = SeedSize;

	// Remap the vector to seed space
	Vector3f rpos = position * (float)SeedSize;

	for (unsigned int i = 0; i < m_Octaves; i++)
	{
		result += amplitude * SampleAtFrequency(rpos, frequency);
		frequency = frequency >> 1;
		amplitudeAcc += amplitude;
		amplitude *= m_Persistance;
	}

	result /= amplitudeAcc;

	return result;

}

float fa::Perlin::SampleNormalized(const Vector2f & position)
{
	float amplitude = 1.0f;
	float amplitudeAcc = 0.0f;
	float result = 0.0f;
	unsigned int frequency = SeedSize;

	// Remap the vector to seed space
	Vector2f rpos = position * (float)SeedSize;

	for (unsigned int i = 0; i < m_Octaves; i++)
	{
		result += amplitude * SampleAtFrequency(rpos, frequency);
		frequency = frequency >> 1;
		amplitudeAcc += amplitude;
		amplitude *= m_Persistance;
	}

	result /= amplitudeAcc;

	return result;
}

inline float fa::Perlin::GetSeed(size_t x, size_t y, size_t z) const
{
	size_t ix = m_Seed[x % SeedSize];
	size_t iy = m_Seed[(ix + y) % SeedSize];
	size_t iz = m_Seed[(iy + z) % SeedSize];
	return (float)iz / SeedSize;
}

inline float fa::Perlin::GetSeed(size_t x, size_t y) const
{
	size_t ix = m_Seed[x % SeedSize];
	size_t iy = m_Seed[(ix + y) % SeedSize];
	return (float)iy / SeedSize;
}

float fa::Perlin::SampleAtFrequency(const Vector3f & position, unsigned int frequency)
{
	static auto fade = [](float t) -> float
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	};

	Vector3f min = (position / frequency).Floor() * frequency;
	Vector3f max = min + Vector3f(frequency, frequency, frequency);

	float a = GetSeed(min.X, min.Y, min.Z);
	float b = GetSeed(max.X, min.Y, min.Z);
	float c = GetSeed(min.X, max.Y, min.Z);
	float d = GetSeed(max.X, max.Y, min.Z);

	float e = GetSeed(min.X, min.Y, max.Z);
	float f = GetSeed(max.X, min.Y, max.Z);
	float g = GetSeed(min.X, max.Y, max.Z);
	float h = GetSeed(max.X, max.Y, max.Z);

	float u = fade((position.X - min.X) / frequency);
	float v = fade((position.Y - min.Y) / frequency);
	float w = fade((position.Z - min.Z) / frequency);

	// Trilinear interpolation
	return Util::Lerp(
		Util::Lerp(Util::Lerp(a, b, u), Util::Lerp(c, d, u), v), 
		Util::Lerp(Util::Lerp(e, f, u), Util::Lerp(g, h, u), v), 
		w);
}

float fa::Perlin::SampleAtFrequency(const Vector2f & position, unsigned int frequency)
{
	static auto fade = [](float t) -> float
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	};

	Vector2f min = (position / frequency).Floor() * frequency;
	Vector2f max = min + Vector2f(frequency, frequency);

	float a = GetSeed(min.X, min.Y);
	float b = GetSeed(max.X, min.Y);
	float c = GetSeed(min.X, max.Y);
	float d = GetSeed(max.X, max.Y);

	float u = fade((position.X - min.X) / frequency);
	float v = fade((position.Y - min.Y) / frequency);

	// Bilinear interpolation
	return Util::Lerp(Util::Lerp(a, b, u), Util::Lerp(c, d, u), v);
}

