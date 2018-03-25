#include "Util.h"
#include <fstream>

std::stringstream _Log;

GLuint _CreateShader(std::string source, GLenum type)
{
	GLuint shader = 0;
	GLint compileStatus = 0;
	GLint srcLength = 0;
	GLchar** src = new GLchar*[1];
	GLchar infoLog[256];
	GLint infoLogLength = 0;

	src[0] = (GLchar*) source.c_str();
	fa::Util::GetLog() << "Compiling...\n" << src[0] << "\n";

	shader = glCreateShader(type);
	glShaderSource(shader, 1, src, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus)
	{
		glGetShaderInfoLog(shader, 256, &infoLogLength, infoLog);
		fa::Util::GetLog() << "InfoLog:\n" << infoLog << "\n";
		return 0;
	}

	return shader;
}


std::string fa::Util::ReadFile(std::string fileName)
{
	std::ifstream is;
	std::string result;
	std::string line;
	is.open(fileName.c_str(), std::ios_base::in);

	while (is.good())
	{
		std::getline(is, line);
		result += line + '\n';
	}

	is.close();

	return result;

}

GLuint fa::Util::CreateProgram(std::string vertexSource, std::string fragmentSource)
{	
	GLuint program = glCreateProgram();
	
	GLuint vertexShader = _CreateShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = _CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

	ZeroFail(vertexShader);
	ZeroFail(fragmentShader);

	glFastFail(glAttachShader(program, vertexShader));
	glFastFail(glAttachShader(program, fragmentShader));

	glFastFail(glLinkProgram(program));

	glFastFail(glValidateProgram(program));

	return program;
}

std::stringstream & fa::Util::GetLog()
{
	return _Log;
}
