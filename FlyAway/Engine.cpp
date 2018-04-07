#include "Engine.h"
#include "Util.h"
#include "Scene.h"
#include "GLFW/glfw3.h"
#include "Wavefront.h"
#include "WavefrontModel.h"
#include <chrono>
#include <functional>


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
		
		glfwSetWindowSizeCallback(window, Engine::WindowSizeCallback);

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

int fa::Engine::GetWindowWidth() const
{
	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);
	return width;
}

int fa::Engine::GetWindowHeight() const
{
	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);
	return height;
}

void fa::Engine::Init()
{
	// Initialize Shaders
	LoadShader("basic", "shaders/basic.vert", "shaders/basic.frag");
	LoadShader("deferred", "shaders/deferred.vert", "shaders/deferred.frag");
	LoadShader("deferred_edge", "shaders/deferred_edge.vert", "shaders/deferred_edge.frag");
	LoadShader("deferred_last", "shaders/deferred_last.vert", "shaders/deferred_last.frag");
	LoadShader("deferred_sky", "shaders/deferred_sky.vert", "shaders/deferred_sky.frag");

	// Load models
	LoadModels();

	// Initialize frame buffers
	m_DeferredFB = std::unique_ptr<FrameBuffer>(new FrameBuffer(GetWindowWidth(), GetWindowHeight(), true, 2));
	m_EdgeFB = std::unique_ptr<FrameBuffer>(new FrameBuffer(GetWindowWidth(), GetWindowHeight(), false, 1));
	m_SkyFB = std::unique_ptr<FrameBuffer>(new FrameBuffer(GetWindowWidth(), GetWindowHeight(), false, 1));

	// Initialize Quad for rendering to screen
	CreateQuadVAO();

	// Initialize Scene
	m_Scene = new Scene(this, 300.0f, 6.0f, 1.0f, 300.0f);

	// Initialize sky
	m_Sky = std::unique_ptr<Sky>(new Sky(24.0f));

	// Init GL Parameters
	glEnable(GL_DEPTH_TEST);

}

void fa::Engine::Update(float elapsedTime)
{
	m_Scene->Update(elapsedTime);
	m_Sky->Update(elapsedTime);
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	auto light = m_Sky->GetLight();
	auto topColor = m_Sky->GetTopColor();
	auto horizonColor = m_Sky->GetHorizonColor();
	Vector3f viewDir(0.0, 0.0, -1.0);

	// Deferred step 1: Draw albedo and normals

	{
		glFastFail(glBindFramebuffer(GL_FRAMEBUFFER, m_DeferredFB->GetFrameBuffer()));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw Sky
		// Disable depth test so depth buffer won't get written (I hope)
		{
			glDisable(GL_DEPTH_TEST);

			GLuint program = GetProgram("deferred_sky");

			glFastFail(glUseProgram(program));


			glUniform3fv(glGetUniformLocation(program, "in_LightColor"), 1, (float*)&light.Color);
			glUniform3fv(glGetUniformLocation(program, "in_LightDirection"), 1, (float*)&light.Direction);
			glUniform3fv(glGetUniformLocation(program, "in_ViewDirection"), 1, (float*)&viewDir);
			glUniform3fv(glGetUniformLocation(program, "in_TopColor"), 1, (float*)&topColor);
			glUniform3fv(glGetUniformLocation(program, "in_HorizonColor"), 1, (float*)&horizonColor);
			glUniform2f(glGetUniformLocation(program, "in_Resolution"), GetWindowWidth(), GetWindowWidth());

			glBindVertexArray(m_ScreenQuad.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			glUseProgram(0);
		}

		// Render scene on top of the sky
		{
			glEnable(GL_DEPTH_TEST);
			m_Scene->Render(GetProgram("deferred"));
		}
	}

	// Deferred step 2: Generate edge detection mask
	{
		GLuint program = GetProgram("deferred_edge");

		glFastFail(glBindFramebuffer(GL_FRAMEBUFFER, m_EdgeFB->GetFrameBuffer()));
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		glFastFail(glActiveTexture(GL_TEXTURE0));
		glFastFail(glBindTexture(GL_TEXTURE_2D, m_DeferredFB->GetColorAttachment(1)));

		glFastFail(glActiveTexture(GL_TEXTURE1));
		glFastFail(glBindTexture(GL_TEXTURE_2D, m_DeferredFB->GetDepthAttachment()));

		glFastFail(glUseProgram(program));

		glUniform1i(glGetUniformLocation(program, "in_Texture"), 0);
		glUniform1i(glGetUniformLocation(program, "in_Depth"), 1);
		glUniform2f(glGetUniformLocation(program, "in_Resolution"), GetWindowWidth(), GetWindowHeight());

		glBindVertexArray(m_ScreenQuad.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glUseProgram(0);
	}
	

	// Deferred final step
	{

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glFastFail(glDisable(GL_DEPTH_TEST));
		glFastFail(glClear(GL_COLOR_BUFFER_BIT));

		GLuint program = GetProgram("deferred_last");


		// Albedo
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_DeferredFB->GetColorAttachment(0));

		// Normals
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_DeferredFB->GetColorAttachment(1));

		// Edge
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_EdgeFB->GetColorAttachment(0));

		glFastFail(glUseProgram(program));

		glUniform1i(glGetUniformLocation(program, "in_Albedo"), 0);
		glUniform1i(glGetUniformLocation(program, "in_Normal"), 1);
		glUniform1i(glGetUniformLocation(program, "in_Edge"), 2);
		glUniform3fv(glGetUniformLocation(program, "in_LightColor"), 1, (float*)&light.Color);
		glUniform3fv(glGetUniformLocation(program, "in_LightDirection"), 1, (float*)&light.Direction);

		glFastFail(glBindVertexArray(m_ScreenQuad.VAO));
		glFastFail(glDrawArrays(GL_TRIANGLES, 0, 6));
		glFastFail(glBindVertexArray(0));

		glUseProgram(0);

	}
}

void fa::Engine::Resize()
{
	int width = GetWindowWidth(), height = GetWindowHeight();

	glViewport(0, 0, width, height);

	m_DeferredFB->Resize(width, height);
	m_EdgeFB->Resize(width, height);

}

void fa::Engine::CreateQuadVAO()
{
	std::array<Vector2f, 6> coords = {

		Vector2f{ 0.0f, 0.0f },
		Vector2f{ 1.0f, 0.0f },
		Vector2f{ 1.0f, 1.0f },

		Vector2f{ 0.0f, 0.0f },
		Vector2f{ 1.0f, 1.0f },
		Vector2f{ 0.0f, 1.0f }
	};

	glGenVertexArrays(1, &m_ScreenQuad.VAO);

	glBindVertexArray(m_ScreenQuad.VAO);

	glGenBuffers(1, &m_ScreenQuad.VB);
	glBindBuffer(GL_ARRAY_BUFFER, m_ScreenQuad.VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * 6, (void*)coords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void fa::Engine::WindowSizeCallback(GLFWwindow * window, int width, int height)
{
	m_Instance->Resize();
}

void fa::Engine::LoadModels()
{
	Wavefront cube = Wavefront::Parse(Util::ReadFile("models/cube.obj"));
	Wavefront cypress0 = Wavefront::Parse(Util::ReadFile("models/cypress0.obj"));
	Wavefront rock0 = Wavefront::Parse(Util::ReadFile("models/rock0.obj"));
	Wavefront cactus0 = Wavefront::Parse(Util::ReadFile("models/cactus0.obj"));

	m_Models[Models::RedCube] = WavefrontModel::Create(cube, { { "cube", Vector3f(1.0f, 0.0f, 0.0f) } });
	m_Models[Models::WhiteCube] = WavefrontModel::Create(cube, { { "cube", Vector3f(1.0f, 1.0f, 1.0f) } });

	m_Models[Models::Cypress0LightGreen] = WavefrontModel::Create(
		cypress0,
		{
			{ "leaves", Vector3f::GetColor(114, 163, 41) },
			{ "log", Vector3f::GetColor(92, 43, 43) }
		}
	);


	m_Models[Models::Cypress0DarkGreen] = WavefrontModel::Create(
		cypress0,
		{
			{ "leaves", Vector3f::GetColor(59, 92, 10) },
			{ "log", Vector3f::GetColor(92, 43, 43) }
		}
	);

	m_Models[Models::Rock0LightBrown] = WavefrontModel::Create(rock0, { { "rock", Vector3f::GetColor(166, 145, 123) } });
	m_Models[Models::Rock0DarkBrown] = WavefrontModel::Create(rock0, { { "rock", Vector3f::GetColor(79, 57, 36) } });

	m_Models[Models::Cactus0Green] = WavefrontModel::Create(cactus0, { { "cactus", Vector3f::GetColor(167, 219, 118) } });
	m_Models[Models::Cactus0PaleGreen] = WavefrontModel::Create(cactus0, { { "cactus", Vector3f::GetColor(202, 231, 119)} });
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

fa::FrameBuffer::FrameBuffer(int width, int height, bool depthAttachment, int colorAttachments) :
	m_Depth(0)
{
	glGenFramebuffers(1, &m_FB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
	if (depthAttachment)
	{
		glFastFail(glGenTextures(1, &m_Depth));
		glFastFail(glBindTexture(GL_TEXTURE_2D, m_Depth));
		glFastFail(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
		glFastFail(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glFastFail(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		glFastFail(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_Depth, 0));
	}

	
	if (colorAttachments > 0)
	{
		std::vector<GLuint> drawBuffers;
		for (int i = 0; i < colorAttachments; i++)
		{
			GLuint color;
			glFastFail(glGenTextures(1, &color));
			glFastFail(glBindTexture(GL_TEXTURE_2D, color));
			glFastFail(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
			glFastFail(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			glFastFail(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			glFastFail(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, color, 0));
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			m_Color.push_back(color);
		}
		

		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	}

		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Resize(width, height);
}

fa::FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_FB);

	if (m_Depth != 0)
	{
		glDeleteTextures(1, &m_Depth);
	}

	for (auto color : m_Color)
	{
		glDeleteTextures(1, &color);
	}
}

GLuint fa::FrameBuffer::GetFrameBuffer() const
{
	return m_FB;
}

GLuint fa::FrameBuffer::GetDepthAttachment() const
{
	return m_Depth;
}

GLuint fa::FrameBuffer::GetColorAttachment(int index) const
{
	return m_Color[index];
}

void fa::FrameBuffer::Resize(int width, int height)
{
	glFastFail(glBindTexture(GL_TEXTURE_2D, m_Depth));
	glFastFail(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

	for (auto color : m_Color)
	{
		glFastFail(glBindTexture(GL_TEXTURE_2D, color));
		glFastFail(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
}
