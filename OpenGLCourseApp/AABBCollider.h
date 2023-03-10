#pragma once
#include <glm\glm.hpp>
#include "Line.h"

class AABBCollider
{


public:
	AABBCollider();

	void SetPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 GetPosition() { return position; }
	void SetSize(float sizeX, float sizeY, float sizeZ);
	bool Intersect(AABBCollider* b);
	void CalculateLimits();
	void CalculateColliderLines();

	~AABBCollider();

	glm::vec3 position;
	float sizeX;
	float sizeY;
	float sizeZ;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
	Line* colliderLines;

private:
	

};

