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
	auto engine = fa::Engine::GetInstance();
	engine->Start();
	return 0;
}