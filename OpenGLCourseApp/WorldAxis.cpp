#include "WorldAxis.h"

WorldAxis::WorldAxis()
{
	GLfloat xAxisVertices[] = {
		0.0f, 0.0f, 0.0f,
		axisSize, 0.0f, 0.0f,
	};

	GLfloat yAxisVertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, axisSize, 0.0f,
	};

	GLfloat zAxisVertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, axisSize,
	};

	unsigned int indices[] = {
		0, 1,
	};

	xAxis = Line();
	xAxis.Create(xAxisVertices, indices, sizeof(xAxisVertices), sizeof(indices));
	xAxis.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));

	yAxis = Line();
	yAxis.Create(yAxisVertices, indices, sizeof(yAxisVertices), sizeof(indices));
	yAxis.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));

	zAxis = Line();
	zAxis.Create(zAxisVertices, indices, sizeof(zAxisVertices), sizeof(indices));
	zAxis.SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

}

void WorldAxis::Render(GLint uniformColor)
{
	glUniform3fv(uniformColor, 1, &xAxis.GetColor()[0]);
	xAxis.Render();
	glUniform3fv(uniformColor, 1, &yAxis.GetColor()[0]);
	yAxis.Render();
	glUniform3fv(uniformColor, 1, &zAxis.GetColor()[0]);
	zAxis.Render();
}

WorldAxis::~WorldAxis()
{
}
