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
	glm::mat4 GetModel();

	~Model();

private:

	void LoadNode(aiNode *node, const aiScene *scene);
	void LoadMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterials(const aiScene *scene);

	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;
	glm::mat4 model;
};

