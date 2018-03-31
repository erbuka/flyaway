#include "Util.h"
#include "Engine.h"
#include "Vecmath.h"
#include "MatrixStack.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <chrono>

#include "Wavefront.h"
#include <fstream>

int main(int argc, char ** argv)
{	
	GLFWwindow * window = nullptr;

	fa::Util::ZeroFail(glfwInit());
	fa::Util::ZeroFail(window = glfwCreateWindow(fa::CWidth, fa::CHeight, "Test", nullptr, nullptr));
	glfwMakeContextCurrent(window);
	fa::Util::NotZeroFail(glewInit());
	fa::Engine engine(window);

	engine.Start();


	return 0;
}