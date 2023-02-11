#pragma once
#include "Line.h"
class WorldAxis
{
public:
	WorldAxis();
	void Render(GLint uniformColor);

	~WorldAxis();
private:
	Line xAxis;
	Line yAxis;
	Line zAxis;

	const float axisSize = 50.0f;
};

