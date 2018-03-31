#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include "GL/glew.h"

#define glFastFail(f) { f; Util::NotZeroFail(glGetError()); }

namespace fa
{
	constexpr int CWidth = 1280;
	constexpr int CHeight = 768;

	template<typename T>
	class Interpolator
	{
	public:

		Interpolator(std::initializer_list<std::pair<float, T>> values)
		{
			m_Values = values;
		}

		T Sample(float t)
		{
			for (int i = 0; i < m_Values.size() - 1; i++)
			{
				std::pair<float, T>& v0 = m_Values[i];
				std::pair<float, T>& v1 = m_Values[i + 1];

				if (t >= v0.first && t < v1.first)
				{
					return Util::Lerp(v0.second, v1.second, (t - v0.first) / (v1.first - v0.first));
				}
			}

			return T();
		}
	private:
		std::vector<std::pair<float, T>> m_Values;
	};

	class Random
	{
	public:
		template<typename T>
		static T& Next(std::vector<T>& v)
		{
			return v[rand() % v.size()];
		}

		template<typename T>
		static T NextValue()
		{
			return (T)((double)rand() / RAND_MAX);
		}

		template<typename T>
		static T NextValue(T min, T max)
		{
			return min + (NextValue<T>() * (max - min));
		}

		template<typename T>
		static T NextFit(T itemLength, T min, T max)
		{
			return NextValue<T>(min, max - itemLength);
		}
	};

	class Util
	{
	public:

		template<typename T>
		static inline T Clamp(T x, T a, T b)
		{
			return x < a ? a : (x > b ? b : x);
		}

		template<typename T>
		static void NotZeroFail(T result)
		{
			if (result)
			{
				std::cout << L"Fail at " << __FILEW__ << L": " << __LINE__ << std::endl;
				std::cout << GetLog().str() << std::endl;
			}
		}

		template<typename T>
		static void ZeroFail(T result)
		{
			if (!result)
			{
				std::cout << L"Fail at " << __FILEW__ << L": " << __LINE__ << std::endl;
				std::cout << GetLog().str() << std::endl;
			}
		}

		template<typename T, typename K>
		static inline T Lerp(T a, T b, K t)
		{
			return (T)(a * (1.0 - t) + b * t);
		}


		static std::string ReadFile(std::string fileName);
		static GLuint CreateProgram(std::string vertexSource, std::string fragmentSource);

		static std::stringstream& GetLog();

	};
}