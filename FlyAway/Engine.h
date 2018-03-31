#pragma once

#include "GL/glew.h"
#include <map>


struct GLFWwindow;

namespace fa
{

	namespace Models
	{
		constexpr char * Cube = "cube";
	}

	class Scene;
	class WavefrontModel;

	class Engine
	{
	public:

		Engine(GLFWwindow * window);
		~Engine();

		void Start();

		GLuint GetProgram(std::string key);

	private:


		void Init();
		void Dispose();
		void Update(float elapsedTime);
		void Render();

		void LoadModels();

		void LoadShader(std::string key, std::string vsFile, std::string fsFile);

		Scene * m_Scene;
		std::map<std::string, GLuint> m_Programs;
		std::map<std::string, WavefrontModel*> m_Models;
		GLFWwindow * m_Window;
	};
}