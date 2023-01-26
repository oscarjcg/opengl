#include "AABBCollider.h"

AABBCollider::AABBCollider()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	minX = 0.0f;
	minY = 0.0f;
	minZ = 0.0f;
	maxX = 0.0f;
	maxY = 0.0f;
	maxZ = 0.0f;

	sizeX = 0.0f;
	sizeY = 0.0f;
	sizeZ = 0.0f;
}

void AABBCollider::SetSize(int sizeX, int sizeY, int sizeZ)
{
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;

	CalculateLimits();
}

bool AABBCollider::Intersect(AABBCollider* b)
{
	return (
		minX <= b->maxX &&
		maxX >= b->minX &&
		minY <= b->maxY &&
		maxY >= b->minY &&
		minZ <= b->maxZ &&
		maxZ >= b->minZ
	);
}

void AABBCollider::CalculateLimits()
{
	float half = sizeX / 2.0f;
	minX = position.x - half;
	maxX = position.x + half;

	half = sizeY / 2.0f;
	minY = position.y - half;
	maxY = position.y + half;

	half = sizeZ / 2.0f;
	minZ = position.z - half;
	maxZ = position.z + half;
}

AABBCollider::~AABBCollider()
{
}
