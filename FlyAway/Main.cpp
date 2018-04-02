#include "Util.h"
#include "Engine.h"
#include "Vecmath.h"
#include "MatrixStack.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <chrono>

#include "Wavefront.h"
#include <fstream>


class cTest
{
public:
	using node = fa::RandomTreeNode<int>;
	cTest()
	{
		// Test

		std::shared_ptr<node> n1(new node({ 1 }));
		std::shared_ptr<node> n2(new node({ 2 }));
		n = std::shared_ptr<node>(new node("test", n1, n2));
	}
	~cTest()
	{

	}
private:
	std::shared_ptr<node> n;
};

void test()
{
	cTest * t = new cTest();
	delete t;
}

int main(int argc, char ** argv)
{	

	test();

	system("pause");

	// Fine test
	auto engine = fa::Engine::GetInstance();
	engine->Start();
	return 0;
}