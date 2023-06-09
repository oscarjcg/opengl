#pragma once

#include <vector>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "Texture.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "CommonValues.h"
#include <math.h>
#include "AABBCollider.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();

	void Translate(glm::vec3 values);
	void Rotate(float angle, glm::vec3 values);
	void Scale(glm::vec3 values);
	void SetModel(glm::mat4 value);
	void SetType(int t);
	int GetType() { return type; }
	glm::mat4 GetModel();
	void Update(GLfloat deltaTime);
	void SetDirection(glm::vec3 dir) { direction = dir; }
	void SetPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 GetPosition() { return position; }
	void SetMoveSpeed(GLfloat speed) { moveSpeed = speed; }
	void SetCollider(float sizeX, float sizeY, float sizeZ);
	AABBCollider* GetCollider() { return collider; }
	void Bounce(Model* other);
	float CalculateAnglePlaneDirection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	glm::vec3 CalculateIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	float CalculateDistance(glm::vec3 p1, glm::vec3 p2);
	void ApplyCollisionType(int type);
	void CollisionTop();
	void CollisionBack();
	void CollisionLeft();
	void CollisionRight();
	void CollisionFront();
	void CollisionBottom();
	void SetName(std::string newName) { name = newName; }
	std::string GetName() { return name; }
	GLfloat GetYaw() { return yaw; }
	GLfloat GetPitch() { return pitch; }

	~Model();

	std::vector<glm::vec3> debugPoints;

private:

	void LoadNode(aiNode *node, const aiScene *scene);
	void LoadMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterials(const aiScene *scene);


	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;
	AABBCollider* collider;
	

	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 direction;
	GLfloat moveSpeed;
	int type;
	GLfloat yaw;
	GLfloat pitch;

	int nBounces;

	std::string name;
};

