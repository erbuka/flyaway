#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <random>
#include "GL/glew.h"

#define glFastFail(f) { f; Util::NotZeroFail(glGetError()); }

namespace fa
{
	constexpr int CWidth = 1280;
	constexpr int CHeight = 768;



	template<typename T>
	constexpr T PI() {
		return T(3.1415926535897932385L);
	}

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

			return m_Values[m_Values.size() - 1].second;
		}
	private:
		std::vector<std::pair<float, T>> m_Values;
	};


	template<typename T>
	class RandomTreeNode
	{
	public:
		using Inputs = std::map<std::string, float>;
		using Pointer = std::shared_ptr<RandomTreeNode<T>>;

		RandomTreeNode(std::initializer_list<T> values) : m_Leaf(true)
		{
			for (auto& v : values)
			{
				m_Values.push_back(v);
			}
		}

		RandomTreeNode(std::string inputName, Pointer left, Pointer right) :
			m_InputName(inputName), m_Left(left), m_Right(right), m_Leaf(false)
		{
		}

		~RandomTreeNode()
		{
		}

		T GetResult(Inputs& inputs)
		{
			float input = inputs.at(m_InputName);
			Pointer selectedNode = Random::NextValue<float>() < input ? m_Left : m_Right;
			return selectedNode->m_Leaf ? selectedNode->GetValue() : selectedNode->GetResult(inputs);
		}

	private:

		T GetValue()
		{
			return Random::Next(m_Values);
		}

		std::vector<T> m_Values;
		bool m_Leaf;
		std::string m_InputName;
		Pointer m_Left, m_Right;
	};

	class Random
	{
	public:
		
		void Initialize()
		{

		}

		template<typename T>
		static T& Next(std::vector<T>& v)
		{
			return v[(int)(v.size() * Rand())];
		}

		template<typename T>
		static inline T NextValue()
		{
			return (T)(Rand());
		}

		template<typename T>
		static inline T NextValue(T min, T max)
		{
			return (T)(min + NextValue<double>() * (max - min));
		}

		template<typename T>
		static inline T NextFit(T itemLength, T min, T max)
		{
			return NextValue<T>(min, max - itemLength);
		}
	private:
		static inline double Rand()
		{
			static std::default_random_engine engine;
			static std::uniform_real_distribution<double> distribution;
			return distribution(engine);
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
		inline static void NotZeroFail(T result)
		{
			if (result)
			{
				std::cout << "Fail at " << __FILE__ << ":" << __LINE__ << " with result: " << result << std::endl;
				std::cout << GetLog().str() << std::endl;
			}
		}

		template<typename T>
		inline static void ZeroFail(T result)
		{
			if (!result)
			{
				std::cout << "Fail at " << __FILE__ << ":" << __LINE__ << " with result: " << result << std::endl;
				std::cout << GetLog().str() << std::endl;
			}
		}

		template<typename T, typename K>
		static inline T Lerp(T a, T b, K t)
		{
			return (T)(a * (1.0 - t) + b * t);
		}

		template<typename T, typename K>
		static inline T CosLerp(T a, T b, K t)
		{
			K t2 = (K)1 - (std::cos((K)2 * t * PI<K>()) + (K)1) / (K)2;
			return Lerp(a, b, t2);
		}

		/*
		template<typename T, typename K>
		static inline Slerp(T a, T b, K t, K a)
		{
			K sa = std::sinf(a);
			return (T)(std::sinf((1.0 - t) * a) / sa + std::sinf(t * a) / sa);
		}
		*/


		static std::string ReadFile(std::string fileName);
		static GLuint CreateProgram(std::string vertexSource, std::string fragmentSource);
		static GLuint CreateProgram(std::string vertexSource, std::string geometrySource, std::string fragmentSource);

		static std::stringstream& GetLog();

	};
}