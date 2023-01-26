#pragma once
#include <glm\glm.hpp>

class AABBCollider
{


public:
	AABBCollider();

	void SetPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 GetPosition() { return position; }
	void SetSize(int sizeX, int sizeY, int sizeZ);
	bool Intersect(AABBCollider* b);
	void CalculateLimits();

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

private:
	

};

