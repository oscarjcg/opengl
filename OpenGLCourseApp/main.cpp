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
#include "GameController.h"

#include "Model.h"



Window mainWindow;
std::vector<Shader> shaderList;

GameController gameController;

Material shinyMaterial;
Material dullMaterial;



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

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);
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

	Shader* shader2 = new Shader();
	//shader2->CreateFromFiles("Shaders/line_shader.vert", "Shaders/line_shader.frag");
	shader2->CreateFromFiles(vShader, fShader);

	shaderList.push_back(*shader1);
	shaderList.push_back(*shader2);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	gameController.init();
	Camera camera;
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);
	gameController.setCamera(&camera);
	
	
	CreateObjects();
	CreateLights();
	CreateShaders();

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
		camera.mouseControl(&gameController, 
			mainWindow.getsMouseKeys(), mainWindow.getsMouseKeysConsumed(),
			mainWindow.getXChange(), mainWindow.getYChange());

		gameController.update(deltaTime);

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
		/*
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		*/

		glm::mat4 viewMatrix = camera.calculateViewMatrix();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		std::vector<Model*> models = gameController.getModels();
		for (size_t i = 0; i < 1; i++)
		{
			models[i]->Update(deltaTime);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->GetModel()));
			//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			models[i]->RenderModel();
		}


	// ------------------------------------------------

		shaderList[1].UseShader();
		uniformModel = shaderList[1].GetModelLocation();
		uniformProjection = shaderList[1].GetProjectionLocation();
		uniformView = shaderList[1].GetViewLocation();
		uniformEyePosition = shaderList[1].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[1].GetSpecularIntensityLocation();
		uniformShininess = shaderList[1].GetShininessLocation();

		//glm::vec3 lowerLight = camera.getCameraPosition();
		//lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		
		shaderList[1].SetDirectionalLight(&mainLight);
		/*
		shaderList[1].SetPointLights(pointLights, pointLightCount);
		shaderList[1].SetSpotLights(spotLights, spotLightCount);
		*/

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		
		for (size_t i = 1; i < models.size(); i++)
		{
			models[i]->Update(deltaTime);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->GetModel()));
			//shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			models[i]->RenderModel();
		}
		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}


	return 0;
}