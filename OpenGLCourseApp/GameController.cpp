#include "GameController.h"

GameController::GameController()
{
	
}

void GameController::init()
{
	CreateObjects();
}


void GameController::Shoot()
{
	
	Model* bulletModel = new Model();
	bulletModel->LoadModel("Models/Bullet.obj");
	bulletModel->SetModel(glm::mat4(1.0f));
	//bulletModel->Translate(camera->getCameraPosition());
	//bulletModel.Scale(glm::vec3(2000.0f, 2000.0f, 2000.0f));
	bulletModel->SetType(MODEL_BULLET);
	bulletModel->SetDirection(camera->getCameraDirection());
	bulletModel->SetPosition(camera->getCameraPosition());
	//bulletModel->SetDirection(glm::vec3(1.0f, 0.0f, 0.0f));
	//bulletModel->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	bulletModel->SetMoveSpeed(3.0f);
	bulletModel->SetCollider(1.0f, 1.0f, 1.0f);

	models.push_back(bulletModel);
	
}



Camera* GameController::getCamera()
{
	return camera;
}

void GameController::update(GLfloat deltaTime)
{
	CheckGameLimits(deltaTime);

	CheckColliders();
}

GameController::~GameController()
{
}

void GameController::CreateObjects()
{

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
	//floorModel.Translate(glm::vec3(-1.0f, -1.0f, 0.0f));
	//floorModel.Rotate(-90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	//floorModel.Scale(glm::vec3(0.4f, 0.4f, 0.4f));
	floorModel.SetType(MODEL_ENVIRONMENT);
	floorModel.SetDirection(glm::vec3(1.0f, 0.0f, 0.0f));
	//floorModel.SetPosition(glm::vec3(-1.0f, -1.0f, 0.0f));
	floorModel.SetMoveSpeed(0.0f);
	floorModel.SetCollider(20.0f, 0.2f, 20.0f);

	Model* bulletModel = new Model();
	bulletModel->LoadModel("Models/Bullet.obj");
	bulletModel->SetModel(glm::mat4(1.0f));
	//bulletModel->Translate(camera->getCameraPosition());
	//bulletModel.Scale(glm::vec3(2000.0f, 2000.0f, 2000.0f));
	bulletModel->SetType(MODEL_ENVIRONMENT);
	bulletModel->SetDirection(glm::vec3(1.0f, 0.0f, 0.0f));
	bulletModel->SetPosition(glm::vec3(5.0f, 7.0f, 10.0f));
	//bulletModel->SetDirection(glm::vec3(1.0f, 0.0f, 0.0f));
	//bulletModel->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	bulletModel->SetMoveSpeed(0.0f);
	bulletModel->SetCollider(3.0f, 10.0f, 3.0f);


	models.push_back(bulletModel);

	//models.push_back(&xwing);
	//models.push_back(&blackhawk);
	models.push_back(&floorModel);
	
}

void GameController::CheckGameLimits(GLfloat deltaTime)
{
	std::vector<int> deleteModels;

	for (size_t i = 0; i < models.size(); i++)
	{
		if (abs(models[i]->GetPosition().x) > limitX) {
			deleteModels.push_back(i);
		} else if (abs(models[i]->GetPosition().y) > limitY) {
			deleteModels.push_back(i);
		}
		else if (abs(models[i]->GetPosition().z) > limitZ) {
			deleteModels.push_back(i);
		}
	}

	for (size_t i = 0; i < deleteModels.size(); i++)
	{
		printf("DELETED CheckGameLimits %f %f %f \n", 
			models[deleteModels[i]]->GetPosition().x,
			models[deleteModels[i]]->GetPosition().y,
			models[deleteModels[i]]->GetPosition().z);
		models[deleteModels[i]]->ClearModel();
		models.erase(models.begin() + deleteModels[i]);
	}
}

void GameController::CheckColliders()
{
	std::vector<int> deleteModels;
	for (size_t i = 0; i < models.size(); i++)
	{
		for (size_t j = i; j < models.size(); j++)
		{

			if (i != j && 
				models[i]->GetCollider()->Intersect(models[j]->GetCollider()))
			{
				//printf("------------------Collider Intersect %d %d \n", i, j);


				if (models[i]->GetType() == MODEL_BULLET && models[j]->GetType() == MODEL_ENVIRONMENT)
				{
					//printf("------------------Collider Bounce %d %d \n", i, j);
					models[i]->Bounce(models[j]);

				}
				if (models[i]->GetType() == MODEL_ENVIRONMENT && models[j]->GetType() == MODEL_BULLET)
				{
					//printf("------------------Collider Bounce %d %d \n", i, j);
					models[j]->Bounce(models[i]);
				}

				/*
				if (std::find(deleteModels.begin(), deleteModels.end(), i) != deleteModels.end()) {
					continue;
				}

				if (models[i]->GetType() == MODEL_BULLET)
				{
					deleteModels.push_back(i);
				}
				if (models[j]->GetType() == MODEL_BULLET)
				{
					deleteModels.push_back(j);
				}
				*/


			}

		}
	}

	if (deleteModels.size() > 0) 
	{
		for (int i = deleteModels.size() - 1; i >= 0; i--)
		{
			//printf("DELETED CheckColliders %i \n", i);
			models[deleteModels[i]]->ClearModel();
			models.erase(models.begin() + deleteModels[i]);
		}
	}
}

