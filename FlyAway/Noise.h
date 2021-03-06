#pragma once

#include "Vecmath.h"
#include <array>
namespace fa
{
	class Noise
	{
	public:
		Noise(float domainSize) : m_DomainSize(domainSize) {}
		virtual ~Noise() {}
		float GetDomainSize() { return m_DomainSize; }
		float Sample(const Vector3f& point);
		float Sample(const Vector2f& point);
	protected:
		virtual float SampleNormalized(const Vector3f& point) = 0;
		virtual float SampleNormalized(const Vector2f& point) = 0;
	private:
		float Normalize(float x);
		float m_DomainSize;
	};

	class Perlin : public Noise
	{
	public:
		Perlin(float domainSize);
		~Perlin();
		void NewSeed();
		void SetPersistance(float persistance) { m_Persistance = persistance; }
		void SetOctaves(unsigned int octaves) { m_Octaves = octaves; }

	protected:
		virtual float SampleNormalized(const Vector3f& position) override;
		virtual float SampleNormalized(const Vector2f& position) override;

	private:

		static constexpr size_t SeedSize = 65536;
		static constexpr size_t MaxOctaves = 8;

		float m_Persistance = 0.5f;
		unsigned int m_Octaves = MaxOctaves;

		std::array<float, SeedSize> m_Seed;

		float GetSeed(size_t x, size_t y, size_t z) const;
		float GetSeed(size_t x, size_t y) const;
		float SampleAtFrequency(const Vector3f& position, unsigned int fequency);
		float SampleAtFrequency(const Vector2f& position, unsigned int fequency);
	};
}