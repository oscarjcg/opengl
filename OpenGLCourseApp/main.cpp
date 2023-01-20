#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Shader> shaderList;
std::vector<Model*> models;
Camera camera;

Material shinyMaterial;
Material dullMaterial;

Model xwing;
Model blackhawk;
Model floorModel;
Model bulletModel;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


void CreateObjects() 
{
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	xwing = Model();
	xwing.LoadModel("Models/x-wing.obj");
	xwing.SetModel(glm::mat4(1.0f));
	xwing.Translate(glm::vec3(-7.0f, 0.0f, 10.0f));
	xwing.Scale(glm::vec3(0.006f, 0.006f, 0.006f));

	blackhawk = Model();
	blackhawk.LoadModel("Models/uh60.obj");
	blackhawk.SetModel(glm::mat4(1.0f));
	blackhawk.Translate(glm::vec3(-3.0f, 2.0f, 0.0f));
	blackhawk.Rotate(-90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	blackhawk.Scale(glm::vec3(0.4f, 0.4f, 0.4f));

	floorModel = Model();
	floorModel.LoadModel("Models/floor.obj");
	floorModel.SetModel(glm::mat4(1.0f));
	floorModel.Translate(glm::vec3(-1.0f, -1.0f, 0.0f));
	//floorModel.Rotate(-90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//floorModel.Scale(glm::vec3(0.4f, 0.4f, 0.4f));

	bulletModel = Model();
	bulletModel.LoadModel("Models/Bullet.obj");
	bulletModel.SetModel(glm::mat4(1.0f));
	bulletModel.Translate(glm::vec3(-5.0f, 0.0f, 0.0f));
	//bulletModel.Scale(glm::vec3(2000.0f, 2000.0f, 2000.0f));

	//models.push_back(&xwing);
	//models.push_back(&blackhawk);
	models.push_back(&floorModel);
	models.push_back(&bulletModel);
	
}

void CreateLights()
{
	mainLight = DirectionalLight(
		1.0f, 1.0f, 1.0f,
		0.3f, 0.6f,
		0.0f, 0.0f, -1.0f);


	pointLights[0] = PointLight(
		0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(
		0.0f, 1.0f, 0.0f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;


	spotLights[0] = SpotLight(
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	
	CreateShaders();
	CreateObjects();
	CreateLights();
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();


		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		for (size_t i = 0; i < models.size(); i++)
		{
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->GetModel()));
			shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			models[i]->RenderModel();
		}


		glUseProgram(0);

		mainWindow.swapBuffers();
	}


	return 0;
}