#include "Model.h"

Model::Model()
{
	// TODO Not working
	model = glm::mat4(1.0f);
	type = MODEL_ENVIRONMENT;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	direction = glm::vec3(0.0f, 0.0f, 1.0f);
	moveSpeed = 1.0f;
	nBounces = 0;
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

// TODO Review collision. Should be other object collider?
void Model::Bounce(Model* other)
{
	if (nBounces > 1)
		return;
	

	std::vector<glm::vec3> intersections;
	// Get sides planes and line (Object position, direction)

	glm::vec3 p1 = glm::vec3(other->collider->minX, other->collider->maxY, other->collider->minZ); // Top side
	glm::vec3 p2 = glm::vec3(other->collider->minX, other->collider->maxY, other->collider->maxZ);
	glm::vec3 p3 = glm::vec3(other->collider->maxX, other->collider->maxY, other->collider->minZ);
	glm::vec3 intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);
	
	printf("top intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	p1 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->minZ); // Back side
	p2 = glm::vec3(other->collider->minX, other->collider->maxY, other->collider->minZ);
	p3 = glm::vec3(other->collider->maxX, other->collider->minY, other->collider->minZ);
	intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);

	printf("back intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	p1 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->maxZ); // Front side
	p2 = glm::vec3(other->collider->maxX, other->collider->maxY, other->collider->maxZ);
	p3 = glm::vec3(other->collider->maxX, other->collider->minY, other->collider->maxZ);
	intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);

	printf("front intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	p1 = glm::vec3(other->collider->maxX, other->collider->minY, other->collider->minZ); // Left side
	p2 = glm::vec3(other->collider->maxX, other->collider->maxY, other->collider->minZ);
	p3 = glm::vec3(other->collider->maxX, other->collider->minY, other->collider->maxZ);
	intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);

	printf("left intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	p1 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->minZ); // Right side
	p2 = glm::vec3(other->collider->minX, other->collider->maxY, other->collider->minZ);
	p3 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->maxZ);
	intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);

	printf("right intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	p1 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->minZ); // Bottom side
	p2 = glm::vec3(other->collider->maxX, other->collider->minY, other->collider->minZ);
	p3 = glm::vec3(other->collider->minX, other->collider->minY, other->collider->maxZ);
	intersectionPoint = CalculateIntersection(p1, p2, p3);
	intersections.push_back(intersectionPoint);

	printf("bottom intersection  %f %f %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);

	//debugPoints = intersections;

	// Distances from intersections to object
	std::vector<GLfloat> distances;

	int minIndex = 0;
	float min = INT_MAX;
	for (size_t i = 0; i < intersections.size(); i++)
	{
		float distance = CalculateDistance(position, intersections[i]);
		distances.push_back(distance);
		printf("distance  %f\n", distance);
		if (min > distance) {
			min = distance;
			minIndex = i;
		}
	}


	// Get collision side

	
	switch (minIndex)
	{
		case COLLISION_TOP:
			printf("------------------COLLISION_TOP \n");
			CollisionTop();
			break;
		case COLLISION_BACK:
			printf("------------------COLLISION_BACK \n");
			CollisionBack();
			break;
		case COLLISION_FRONT:
			printf("------------------COLLISION_FRONT \n");
			CollisionFront();
			break;
		case COLLISION_LEFT:
			printf("------------------COLLISION_LEFT \n");
			CollisionLeft();
			break;
		case COLLISION_RIGHT:
			printf("------------------COLLISION_RIGHT \n");
			CollisionRight();
			break;
		case COLLISION_BOTTOM:
			printf("------------------COLLISION_BOTTOM \n");
			CollisionBottom();
			break;
		default:
			break;
	}
	

	nBounces++;
}

float Model::CalculateAnglePlaneDirection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	// Calculate plane equation (ax ,by, cz, k = 0) from 3 points
	// Get vector p1p2 = p1 - p2 and p1p3 = p1 - p3
	// Get normal = p1p2 x p1p3
	// Equation: normal.x * x + normal.y * y + normal.z * z + k = 0

	glm::vec3 p1p2 = glm::vec3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
	glm::vec3 p1p3 = glm::vec3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
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

glm::vec3 Model::CalculateIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 p1p2 = glm::vec3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
	glm::vec3 p1p3 = glm::vec3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
	glm::vec3 normal = glm::normalize(glm::cross(p1p2, p1p3));

	GLfloat k = -normal.x * p1.x - normal.y * p1.y - normal.z * p1.z;

	// Intersection points with sides TODO check parallel error?
	GLfloat t = (-k - normal.x * position.x - normal.y * position.y - normal.z * position.z) /
		(normal.x * direction.x + normal.y * direction.y + normal.z * direction.z);

	glm::vec3 intersection = glm::vec3(position.x + t * direction.x, position.y + t * direction.y, position.z + t * direction.z);
	return intersection;
}

float Model::CalculateDistance(glm::vec3 p1, glm::vec3 p2)
{
	return sqrt((pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2)));
}

void Model::ApplyCollisionType(int type)
{
	
}

void Model::CollisionTop()
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

void Model::CollisionBack()
{
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ); // TODO Check error in tio side!!
	GLfloat angleY = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xz

	p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	p2 = glm::vec3(collider->minX, collider->maxY, collider->minZ);
	p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleX = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xy

	yaw = -angleX; // x axis
	pitch = angleY; // y axis

	if (direction.y < 0)
		pitch = -pitch;

	if (direction.x < 0)
		yaw = -180 + -yaw;


	printf("ANGLE yaw x %f \n", yaw);
	printf("ANGLE pitch y %f \n", pitch);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
}


void Model::CollisionLeft()
{
	
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ); 
	GLfloat angleY = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xz

	p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	p2 = glm::vec3(collider->minX, collider->maxY, collider->minZ);
	p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleX = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xy

	yaw = angleX; // x axis
	pitch = angleY; // y axis

	if (direction.x < 0 && direction.z < 0)
		yaw = -yaw;

	if (direction.y < 0)
		pitch = -pitch;


	printf("ANGLE yaw x %f \n", yaw);
	printf("ANGLE pitch y %f \n", pitch);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
	
}

void Model::CollisionRight()
{
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleY = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xz

	p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	p2 = glm::vec3(collider->minX, collider->maxY, collider->minZ);
	p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleX = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xy

	yaw = angleX; // x axis
	pitch = angleY; // y axis

	
	if (direction.x > 0 && direction.z > 0)
		yaw = 180 - yaw;

	if (direction.x > 0 && direction.z < 0)
		yaw = -180 + yaw;

	if (direction.y < 0)
		pitch = -pitch;

	printf("ANGLE yaw x %f \n", yaw);
	printf("ANGLE pitch y %f \n", pitch);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
}

void Model::CollisionFront()
{
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleY = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xz

	p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	p2 = glm::vec3(collider->minX, collider->maxY, collider->minZ);
	p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
	GLfloat angleX = CalculateAnglePlaneDirection(p1, p2, p3); // Plane xy

	yaw = angleX; // x axis
	pitch = angleY; // y axis

	if (direction.x < 0 && direction.z < 0)
		yaw = 180 - yaw;

	if (direction.y < 0)
		pitch = -pitch;

	printf("ANGLE yaw x %f \n", yaw);
	printf("ANGLE pitch y %f \n", pitch);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
}

void Model::CollisionBottom()
{
	glm::vec3 p1 = glm::vec3(collider->minX, collider->minY, collider->minZ);
	glm::vec3 p2 = glm::vec3(collider->minX, collider->minY, collider->maxZ);
	glm::vec3 p3 = glm::vec3(collider->maxX, collider->minY, collider->minZ);
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

	if (direction.y > 0)
		pitch = -pitch;

	printf("ANGLE yaw x %f \n", yaw);
	printf("ANGLE pitch y %f \n", pitch);

	// Calculate new direccion
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
}

Model::~Model()
{
}
