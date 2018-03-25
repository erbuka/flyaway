#pragma once

#include <iostream>
#include <sstream>
#include "GL/glew.h"
#include <functional>

#define glFastFail(f) { f; Util::NotZeroFail(glGetError()); }

namespace fa
{
	constexpr int CWidth = 1280;
	constexpr int CHeight = 768;

	class Util
	{
	public:

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
			return (T)((1.0 - t) * a + t * b);
		}

		static std::string ReadFile(std::string fileName);
		static GLuint CreateProgram(std::string vertexSource, std::string fragmentSource);

		static std::stringstream& GetLog();

	};
}