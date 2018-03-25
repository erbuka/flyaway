#include "Engine.h"
#include "Util.h"
#include "Scene.h"
#include "GLFW/glfw3.h"
#include <chrono>

fa::Engine::Engine(GLFWwindow * window) : m_Window(window)
{
	Init();
}

fa::Engine::~Engine()
{
	Dispose();
}

void fa::Engine::Start()
{
	char title[256];
	auto prevTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(m_Window))
	{
		auto currTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> deltaTime = currTime - prevTime;
		prevTime = currTime;

		sprintf_s(title, "FPS: %d", (int)(1 / deltaTime.count()));

		glfwSetWindowTitle(m_Window, title);

		Update(deltaTime.count());
		Render();

		glfwSwapBuffers(m_Window);

		glfwPollEvents();

	}
}

void fa::Engine::Init()
{
	// Initialize Shaders
	LoadShader("basic", "shaders/basic.vert", "shaders/basic.frag");

	// Initialize Scene
	m_Scene = new Scene(this, 50, 50, 1.0f);


	// Init GL Parameters
	glEnable(GL_DEPTH_TEST);
}

void fa::Engine::Update(float elapsedTime)
{
	m_Scene->Update(elapsedTime);
}

GLuint fa::Engine::GetProgram(std::string key)
{
	auto program = m_Programs.find(key);
	Util::NotZeroFail(program == m_Programs.end());
	return (*program).second;
}

void fa::Engine::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_Scene->Render();
}

void fa::Engine::Dispose()
{
	delete m_Scene;
	for (auto program : m_Programs)
	{
		glDeleteProgram(program.second);
	}
}

void fa::Engine::LoadShader(std::string key, std::string vsFile, std::string fsFile)
{
	std::string vsSource = Util::ReadFile(vsFile);
	std::string fsSource = Util::ReadFile(fsFile);
	m_Programs.insert({ key, Util::CreateProgram(vsSource, fsSource) });
}
