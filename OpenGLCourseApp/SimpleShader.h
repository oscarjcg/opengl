#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include "CommonValues.h"
#include "BaseShader.h"

class SimpleShader : public BaseShader
{
public:
	SimpleShader();

	GLuint GetColorLocation();

	~SimpleShader();

private:
	GLuint uniformColor;


	void CompileShader(const char* vertexCode, const char* fragmentCode);
};

