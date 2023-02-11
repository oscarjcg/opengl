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

	colliderLines = new Line();
}

void AABBCollider::SetSize(float sizeX, float sizeY, float sizeZ)
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

	CalculateColliderLines();
}

void AABBCollider::CalculateColliderLines()
{
	GLfloat lineVertices[] = {
		minX, minY, minZ, // 0
		maxX, minY, minZ,
		minX, maxY, minZ,
		minX, minY, maxZ,
		maxX, maxY, maxZ, // 4
		minX, maxY, maxZ,
		maxX, minY, maxZ,
		maxX, maxY, minZ,
	};

	unsigned int lineIndices[] = {
		0, 1, // Lower axis
		0, 2,
		0, 3,
		4, 5, // Upper axis
		4, 6,
		4, 7,
		5, 2, // Between axis
		5, 3,
		1, 6,
		1, 7,
		2, 7,
		3, 6,
		2, 1, // Faces
		7, 6,
		4, 3,
		5, 0,
		0, 6,
		5, 7
	};

	
	delete colliderLines;
	colliderLines = 0;

	colliderLines = new Line();
	colliderLines->Create(lineVertices, lineIndices, sizeof(lineVertices), sizeof(lineIndices));
	colliderLines->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
	
}

AABBCollider::~AABBCollider()
{
	delete colliderLines;
	colliderLines = 0;
}
