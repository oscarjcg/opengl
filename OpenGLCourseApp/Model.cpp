#include "Model.h"

Model::Model()
{
	// TODO Not working
	model = glm::mat4(1.0f);
	type = MODEL_ENVIRONMENT;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, 1.0f);
	moveSpeed = 1.0f;
}

void Model::RenderModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];

		if (materialIndex < textureList.size() && textureList[materialIndex])
		{
			textureList[materialIndex]->UseTexture();
		}

		meshList[i]->RenderMesh();
	}
}

void Model::LoadModel(const std::string & fileName)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!scene)
	{
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
		return;
	}

	LoadNode(scene->mRootNode, scene);

	LoadMaterials(scene);
}

void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		if (mesh->mTextureCoords[0])
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	meshList.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene * scene)
{
	textureList.resize(scene->mNumMaterials);
	
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		textureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/") + filename;

				textureList[i] = new Texture(texPath.c_str());

				if (!textureList[i]->LoadTexture())
				{
					printf("Failed to load texture at: %s\n", texPath);
					delete textureList[i];
					textureList[i] = nullptr;
				}
			}
		}

		if (!textureList[i])
		{
			textureList[i] = new Texture("Textures/plain.png");
			textureList[i]->LoadTextureA();
		}
	}
}


void Model::ClearModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < textureList.size(); i++)
	{
		if (textureList[i])
		{
			delete textureList[i];
			textureList[i] = nullptr;
		}
	}

	delete collider;
	collider = 0;
}

void Model::Translate(glm::vec3 values)
{
	//model = glm::mat4(1.0f);
	model = glm::translate(model, values);
}

void Model::Rotate(float angle, glm::vec3 values)
{
	model = glm::rotate(model, angle, values);
}

void Model::Scale(glm::vec3 values)
{
	model = glm::scale(model, values);
}

void Model::SetModel(glm::mat4 value)
{
	model = value;
	type = MODEL_BULLET;
}

void Model::SetType(int t)
{
	type = t;
}

glm::mat4 Model::GetModel()
{
	return model;
}

void Model::Update(GLfloat deltaTime)
{
	double eps = (moveSpeed * deltaTime) /
		(sqrt(pow(static_cast<double>(direction.x), 2) +
			pow(static_cast<double>(direction.y), 2) +
			pow(static_cast<double>(direction.z), 2))
			);

	float x = position.x + (eps * direction.x);
	float y = position.y + (eps * direction.y);
	float z = position.z + (eps * direction.z);
	glm::vec3 newPos = glm::vec3(x, y, z);
	position = newPos;

	model = glm::mat4(1.0f);
	if (type == MODEL_BULLET)
		Translate(newPos);

	collider->SetPosition(newPos);
	collider->CalculateLimits();
}



void Model::SetCollider(float sizeX, float sizeY, float sizeZ)
{
	AABBCollider* collider = new AABBCollider();
	collider->SetPosition(position);
	collider->SetSize(sizeX, sizeY, sizeZ);
	this->collider = collider;
}

void Model::Bounce()
{
	// Calculate angle
	
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->maxY, collider->minZ);
	GLfloat angleY = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xz

	p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	p2 = glm::vec3(collider->minX, collider->maxY, collider->minZ);
	p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleX = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xy

	yaw = angleX; // x axis
	pitch = angleY; // y axis

	if (direction.z < 0 && direction.x > 0)
		yaw = -yaw;

	if (direction.x < 0 && direction.z > 0)
		yaw = 180.0f - yaw;

	if (direction.x < 0 && direction.z < 0)
		yaw = -180.0f + yaw;

	printf("ANGLE yaw x %f \n", yaw);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);

}

float Model::CalculateAnglePlaneDirection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	// Calculate plane equation (ax ,by, cz, k = 0) from 3 points
	// Get vector p1p2 = p1 - p2 and p1p3 = p1 - p3
	// Get normal = p1p2 x p1p3
	// Equation: normal.x * x + normal.y * y + normal.z * z + k = 0

	glm::vec3 p1p2 = glm::vec3(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
	glm::vec3 p1p3 = glm::vec3(p1.x - p3.x, p1.y - p3.y, p1.z - p3.z);
	glm::vec3 normal = glm::normalize(glm::cross(p1p2, p1p3));

	GLfloat k = -normal.x * p1.x - normal.y * p1.y - normal.z * p1.z;

	// Calculate angle between plane and direction
	GLfloat sineAngle =
		abs(normal.x * direction.x + normal.y * direction.y + normal.z * direction.z) /
		(sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2)) *
			sqrt(pow(direction.x, 2) + pow(direction.y, 2) + pow(direction.z, 2))
		);

	return glm::degrees(asin(sineAngle));
}

Model::~Model()
{
}
