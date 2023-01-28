#pragma once
#include <GL/glew.h>
#include <glm\glm.hpp>

class Line
{
public:
	Line();
	void Create(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
	void Render();
	void SetColor(glm::vec3 color) { this->color = color; }
	glm::vec3 GetColor() { return color; }

private:
	GLuint VBO, VAO, IBO;
	unsigned int numOfIndices;
	glm::vec3 color;
};

