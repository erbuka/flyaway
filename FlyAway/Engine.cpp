#include "Engine.h"
#include "Util.h"
#include "Scene.h"
#include "GLFW/glfw3.h"
#include "Wavefront.h"
#include "WavefrontModel.h"
#include <chrono>


fa::Engine * fa::Engine::m_Instance = nullptr;

fa::Engine::Engine(GLFWwindow * window) : m_Window(window)
{
	
}

fa::Engine * fa::Engine::GetInstance()
{
	if (m_Instance == nullptr)
	{
		GLFWwindow * window;
		fa::Util::ZeroFail(glfwInit());
		fa::Util::ZeroFail(window = glfwCreateWindow(fa::CWidth, fa::CHeight, "Test", nullptr, nullptr));
		glfwMakeContextCurrent(window);
		fa::Util::NotZeroFail(glewInit());
		
		m_Instance = new Engine(window);

	}

	return m_Instance;
}

fa::Engine::~Engine()
{
	Dispose();
}

void fa::Engine::Start()
{
	char title[256];
	auto prevTime = std::chrono::high_resolution_clock::now();

	Init();

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
	LoadShader("toon", "shaders/toon.vert", "shaders/toon.frag");

	// Load models
	LoadModels();

	// Initialize Scene
	m_Scene = new Scene(this, 300.0f, 6.0f, 1.0f, 300.0f);

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

fa::Model * fa::Engine::GetModel(std::string key)
{
	auto model = m_Models.find(key);
	Util::NotZeroFail(model == m_Models.end());
	return (*model).second;
}

void fa::Engine::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_Scene->Render();
}

void fa::Engine::LoadModels()
{
	Wavefront cube = Wavefront::Parse(Util::ReadFile("models/cube.obj"));
	Wavefront cypress0 = Wavefront::Parse(Util::ReadFile("models/cypress0.obj"));

	m_Models[Models::RedCube] = WavefrontModel::Create(cube, { { "cube", Vector3f(1.0f, 0.0f, 0.0f) } });
	m_Models[Models::WhiteCube] = WavefrontModel::Create(cube, { { "cube", Vector3f(1.0f, 1.0f, 1.0f) } });

	m_Models[Models::LightGreenCypress] = WavefrontModel::Create(
		cypress0,
		{
			{ "leaves", Vector3f::GetColor(114, 163, 41) },
			{ "log", Vector3f::GetColor(92, 43, 43) }
		}
	);


	m_Models[Models::DarkGreenCypress] = WavefrontModel::Create(
		cypress0,
		{
			{ "leaves", Vector3f::GetColor(59, 92, 10) },
			{ "log", Vector3f::GetColor(92, 43, 43) }
		}
	);
}

void fa::Engine::Dispose()
{
	delete m_Scene;
	for (auto program : m_Programs)
	{
		glDeleteProgram(program.second);
	}

	for (auto model : m_Models)
	{
		delete model.second;
	}
}

void fa::Engine::LoadShader(std::string key, std::string vsFile, std::string fsFile)
{
	std::string vsSource = Util::ReadFile(vsFile);
	std::string fsSource = Util::ReadFile(fsFile);
	m_Programs.insert({ key, Util::CreateProgram(vsSource, fsSource) });
}
