#pragma once
#include <vector>
#include "Model.h"
#include "GameControllerActions.h"
#include "Camera.h"

class GameController : public GameControllerActions
{
public:

	GameController();
	void init();
	void Shoot();
	Model* AddPlayer(std::string name);
	std::vector<Model*> getModels() { return models; }
	Camera* getCamera();
	void setCamera(Camera* c) { camera = c; }
	void update(GLfloat deltaTime);

	~GameController();

private:
	std::vector<Model*> models;
	const float toRadians = 3.14159265f / 180.0f;
	const float limitX = 20.0f;
	const float limitY = 20.0f;
	const float limitZ = 20.0f;
	
	Camera* camera;

	Model xwing;
	Model blackhawk;
	Model floorModel;
	//Model bulletModel;

	void CreateObjects();
	void CheckGameLimits(GLfloat deltaTime);
	void CheckColliders();
};

