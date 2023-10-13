#pragma once
#include "common.h"
#include <string>

void createProgram(GLuint& program, GLuint& vertShader, GLuint& fragShader, const std::string& _vertPath, const std::string& _fragPath);
void releaseProgram(GLuint& program, GLuint vertShader, GLuint fragShader);

