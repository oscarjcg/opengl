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
#include "SimpleShader.h"
#include "Triangule.h"
#include "Line.h"
#include "WorldAxis.h"

#include "C:\dev\dotnet\ExternalLibs\socket-io-client_x64-windows\include\sio_client.h"
#include <iostream>

#include "json/json.h"
#include <memory>
#include "PlayerListRemote.h"
#include "JsonUtils.h"


using namespace sio;
using namespace std;

Window mainWindow;
std::vector<Shader> shaderList;
SimpleShader* simpleShader;

GameController gameController;

Material shinyMaterial;
Material dullMaterial;

WorldAxis* worldAxis;
Triangule* triangule;
Triangule* triangule2;
Line* line;

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

Camera camera;

sio::client io;
JsonUtils jsonUtils;

string currentPlayerId = "";


string newPositions = "";
string playerDisconnected = "";

void CreateObjects() 
{

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	triangule = new Triangule();
	triangule2 = new Triangule();
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	triangule->Create(vertices, indices, sizeof(vertices), sizeof(indices));
	triangule->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
	triangule2->Create(vertices, indices, sizeof(vertices), sizeof(indices));
	triangule2->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));

	line = new Line();
	GLfloat lineVertices[] = {
		0.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, 10.0f, 0.0f,
		0.0f, 0.0f, 10.0f
	};

	unsigned int lineIndices[] = {
		0, 1,
		0, 2,
		0, 3,
	};
	
	line->Create(lineVertices, lineIndices, sizeof(lineVertices), sizeof(lineIndices));
	line->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

	worldAxis = new WorldAxis();
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

	//Shader* shader2 = new Shader();
	//shader2->CreateFromFiles("Shaders/line_shader.vert", "Shaders/line_shader.frag");
	//shader2->CreateFromFiles(vShader, fShader);

	shaderList.push_back(*shader1);
	//shaderList.push_back(*shader2);

	simpleShader = new SimpleShader();
	simpleShader->CreateFromFiles("Shaders/simple_shader.vert", "Shaders/simple_shader.frag");
}

int ParseJson(string rawJson, Json::Value& root) {
	JSONCPP_STRING err;
	const auto rawJsonLength = static_cast<int>(rawJson.length());
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err)) {
		std::cout << "error" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "sucess" << root["playerId"] << std::endl;

	return 0;
}

int ParseJsonPosition(string rawJson) {
	
	PlayerListRemote playerListRemote;
	playerListRemote.Parse(rawJson);

	gameController.ClearRemoteBullets();
	for (int i = 0; i < playerListRemote.playerListRemote.size(); i++)
	{
		// TODO Memory leak?
		PlayerDataRemote* playerDataRemote = playerListRemote.playerListRemote[i];
		cout << "ParseJsonPosition " << playerDataRemote->id << endl;

		// Player exits, update position
		string playerId = playerDataRemote->id;
			
		bool found = false;
		if (playerId != currentPlayerId) { // Only remote
			float x = playerDataRemote->position.x;
			float y = playerDataRemote->position.y;
			float z = playerDataRemote->position.z;
			glm::vec3 pos = glm::vec3(x, y, z);

			x = playerDataRemote->direction.x;
			y = playerDataRemote->direction.y;
			z = playerDataRemote->direction.z;
			glm::vec3 dir = glm::vec3(x, y, z);
			float yaw = playerDataRemote->direction.yaw;

			std::vector<Model*> models = gameController.getModels();
			for (size_t j = 0; j < models.size(); j++)
			{
				if (MODEL_PLAYER == models[j]->GetType() && models[j]->GetName() == playerId) {
					found = true;
					models[j]->SetPosition(pos);
					models[j]->SetModel(glm::mat4(1.0f));
					models[j]->Translate(models[j]->GetPosition());
					const float toRadians = 3.14159265f / 180.0f; // TODO refactor
						
						
					models[j]->Rotate(yaw * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
					models[j]->Scale(glm::vec3(0.2f, 0.2f, 0.2f));
					//cout << "updated player " << endl;
					break;
				}
			}

			// Player does not exits, create and update position
			if (!found) {
				Model* model = gameController.AddRemotePlayer(playerId);
				model->SetPosition(pos);
				model->SetModel(glm::mat4(1.0f));
				model->Translate(model->GetPosition());
				model->Scale(glm::vec3(0.2f, 0.2f, 0.2f));
				//cout << "created new player " << playerId << endl;
			}

			// Bullets
			for (int j = 0; j < playerDataRemote->bullets.size(); j++)
			{
				BulletData bullet = playerDataRemote->bullets[j];

				glm::vec3 posBullet = glm::vec3(bullet.x, bullet.y, bullet.z);
				Model* model = gameController.AddRemoteBullet(playerId);
				model->SetPosition(posBullet);
				model->SetModel(glm::mat4(1.0f));
				model->Translate(model->GetPosition());
			}
		}
		
	}


	return 0;
}

void CreateNodeClient()
{
	//io.connect("http://localhost:3001");
	io.connect("http://opengl-node.oscarcatarigutierrez.com:3001");


	io.socket()->on("totalConnections", sio::socket::event_listener_aux(
		[&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			//int conn = data->get_map()["totalConections"]->get_int();
			std::cout << "totalConnections " << data->get_int() << endl;
		})
	);

	io.socket()->on("playerId", sio::socket::event_listener_aux(
		[&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			//int conn = data->get_map()["totalConections"]->get_int();
			std::cout << "playerId " << data->get_string() << endl;
			currentPlayerId = data->get_string();
		})
	);

	io.socket()->on("yourPosition", sio::socket::event_listener_aux(
		[&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			string json = jsonUtils.getNewPositionJson(gameController.getModels(), camera, data->get_string(), currentPlayerId);
			io.socket()->emit("newPosition", json);
		})
	);

	io.socket()->on("updatePositions", sio::socket::event_listener_aux(
		[&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			std::cout << "updatePositions " << endl;
			//std::cout << data->get_string();
			newPositions = data->get_string();
		})
	);


	io.socket()->on("playerDisconnected", sio::socket::event_listener_aux(
		[&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			// TODO Porblems if several at the same time?
			std::cout << "playerDisconnected " + data->get_string() << endl;
			//std::cout << data->get_string();
			playerDisconnected = data->get_string();
		})
	);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	gameController.init();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 10.0f, 0.5f);
	gameController.setCamera(&camera);
	
	CreateObjects();
	CreateLights();
	CreateShaders();
	CreateNodeClient();


	double previousTime = glfwGetTime();
	int frameCount = 0;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		frameCount++;
		// If a second has passed.
		if (now - previousTime >= 1.0)
		{
			// TODO draw FPS indicator UI
			/*
			std::cout << "FPS ";
			std::cout << frameCount;
			std::cout << "\n";
			*/

			frameCount = 0;
			previousTime = now;
		}

		// Get + Handle User Input
		glfwPollEvents();


		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(&gameController, 
			mainWindow.getsMouseKeys(), mainWindow.getsMouseKeysConsumed(),
			mainWindow.getXChange(), mainWindow.getYChange());

		// TODO refactor. process events?
		if (newPositions != "") {
			ParseJsonPosition(newPositions);
			newPositions = "";
		}
		if (playerDisconnected != "") {
			gameController.ClearRemotePlayer(playerDisconnected);
		}

		gameController.update(deltaTime);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------- Shader -------------------

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
		

		glm::mat4 viewMatrix = camera.calculateViewMatrix();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		std::vector<Model*> models = gameController.getModels();
		for (size_t i = 0; i < models.size(); i++)
		{
			models[i]->Update(deltaTime);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->GetModel()));
			shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
			models[i]->RenderModel();
		}

		// ------------------- Simple shader -------------------
		simpleShader->UseShader();

		uniformModel = simpleShader->GetModelLocation();
		uniformProjection = simpleShader->GetProjectionLocation();
		uniformView = simpleShader->GetViewLocation();

		glm::mat4 model(1.0f);
		model = glm::mat4(1.0f);

		//model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		

		if (models[models.size() - 1]->debugPoints.size() == 2)
			model = glm::translate(model, models[models.size() - 1]->debugPoints[0]);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3fv(uniformColor, 1, &triangule->GetColor()[0]);

		triangule->Render();

		model = glm::mat4(1.0f);

		//model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0f));
		

	
		if (models[models.size() - 1]->debugPoints.size() == 2)
			model = glm::translate(model, models[models.size() - 1]->debugPoints[1]);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, &triangule2->GetColor()[0]);
		triangule2->Render();




		model = glm::mat4(1.0f);
		uniformColor = simpleShader->GetColorLocation();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, &line->GetColor()[0]); 
		//line->Render();
		 
		for (size_t i = 0; i < models.size(); i++)
		{
			glUniform3fv(uniformColor, 1, &models[i]->GetCollider()->colliderLines->GetColor()[0]);
			models[i]->GetCollider()->colliderLines->Render();
		}

		model = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		worldAxis->Render(uniformColor);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}


	return 0;
}