#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include "CommonValues.h"

class BaseShader
{
public:
	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();

	void UseShader();
	void ClearShader();

protected:
	GLuint shaderID, uniformProjection, uniformModel, uniformView;


	virtual void CompileShader(const char* vertexCode, const char* fragmentCode) = 0;
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};


