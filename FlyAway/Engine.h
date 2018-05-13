#pragma once

#include "GL/glew.h"
#include <map>
#include <vector>
#include <memory>
#include "Sky.h"

struct GLFWwindow;

namespace fa
{

	namespace Models
	{

		constexpr char * WhiteCube = "white_cube";
		constexpr char * RedCube = "red_cube";

		constexpr char * Cypress0LightGreen = "light_green_cypress";
		constexpr char * Cypress0DarkGreen = "dark_green_cypress";

		constexpr char * Rock0LightBrown = "rock0_light_brown";
		constexpr char * Rock0DarkBrown = "rock0_dark_brown";

		constexpr char * Cactus0PaleGreen = "cactus0_pale_green";
		constexpr char * Cactus0Green = "cactus0_green";

		constexpr char * Oak0PaleGreen = "oak0_pale_green";

		constexpr char * XmasTree0DarkGreen = "xmastree0_dark_green";
	}

	class Scene;
	class WavefrontModel;
	class Model;

	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, bool depthAttachment, int colorAttachments);
		~FrameBuffer();

		GLuint GetFrameBuffer() const;
		GLuint GetDepthAttachment() const;
		GLuint GetColorAttachment(int index) const;

		void Resize(int width, int height);
	
	private:
		int m_Width, m_Height;
		GLuint m_FB;
		GLuint m_Depth;

		std::vector<GLuint> m_Color;

	};

	class Engine
	{
	public:

		static Engine * GetInstance();

		~Engine();

		void Start();

		int GetWindowWidth() const;
		int GetWindowHeight() const;

		GLuint GetProgram(std::string key);
		Model * GetModel(std::string key);

	private:


		static constexpr float SightRange = 300.0f;

		Engine(GLFWwindow * window);

		void Init();
		void Dispose();
		void Update(float elapsedTime);
		void Render();
		void Resize();

		void CreateQuadVAO();

		static void WindowSizeCallback(GLFWwindow* window, int width, int height);

		void LoadModels();

		void LoadShader(std::string key, std::string vsFile, std::string fsFile);

		Scene * m_Scene;
		std::map<std::string, GLuint> m_Programs;
		std::map<std::string, WavefrontModel*> m_Models;
		GLFWwindow * m_Window;

		struct {
			GLuint VAO, VB;
		} m_ScreenQuad;

		std::unique_ptr<FrameBuffer> m_DeferredFB, m_EdgeFB;

		std::unique_ptr<Sky> m_Sky;

		static Engine * m_Instance;
	};


}