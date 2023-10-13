#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>

std::string load_file(const std::string& _path) 
{
	std::ifstream file(_path.c_str());
	if (!file.is_open()) throw std::runtime_error("Failed to open file");
	std::string rtn;
	std::string line;

	while (!file.eof())
	{
		std::getline(file, line);
		rtn += line + '\n';
	}

	return rtn;
}

void createProgram(GLuint& program, GLuint& vertShader, GLuint& fragShader, const std::string& _vertPath, const std::string& _fragPath)
{
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	std::string contents = load_file(_vertPath);
	const char* src = contents.c_str();
	glShaderSource(vertShader, 1, &src, NULL);
	glCompileShader(vertShader);
	GLint success = 0;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) throw std::runtime_error("Failed to compile vert shader");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	contents = load_file(_fragPath);
	src = contents.c_str();
	glShaderSource(fragShader, 1, &src, NULL);
	glCompileShader(fragShader);
	success = 0;
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) 
	{
		GLint maxLength = 0;
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);

		std::printf("%s\n", &(errorLog[0]));


		throw std::runtime_error("Failed to compile frag shader");
	} 

	program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);
	glUseProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		throw std::runtime_error("Failed to link shader program");
	}
}

void releaseProgram(GLuint& program, GLuint vertShader, GLuint fragShader)
{
	glDetachShader(program, fragShader);
	glDetachShader(program, vertShader);
	glDeleteShader(fragShader);
	glDeleteShader(vertShader);
	glDeleteProgram(program);
}