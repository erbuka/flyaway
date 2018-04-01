#pragma once

#include "GL/glew.h"
#include <map>


struct GLFWwindow;

namespace fa
{

	namespace Models
	{

		constexpr char * WhiteCube = "white_cube";
		constexpr char * RedCube = "red_cube";

		constexpr char * LightGreenCypress = "light_green_cypress";
		constexpr char * DarkGreenCypress = "dark_green_cypress";
	}

	class Scene;
	class WavefrontModel;
	class Model;

	class Engine
	{
	public:

		static Engine * GetInstance();

		~Engine();

		void Start();

		GLuint GetProgram(std::string key);
		Model * GetModel(std::string key);

	private:

		Engine(GLFWwindow * window);

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

		static Engine * m_Instance;
	};


}