#pragma once

#include <string>
#include <vector>

#include "Camera.h"
#include "Model.h"

using namespace std;

class JsonUtils
{
public:
	string getFieldJson(string key, string value);
	string getJsonObject(std::vector<string> fields);

	string getCameraPositionJson(Camera camera, string time, string playerId);
	string getCameraDirectionJson(Camera camera);
	string getBulletJson(Model* model);
	string getBulletsJson(std::vector<Model*> models);
	string getNewPositionJson(std::vector<Model*> models, Camera camera, string time, string playerId);
private:
	const std::string JSON_X = "x";
	const std::string JSON_Y = "y";
	const std::string JSON_Z = "z";
	const std::string JSON_PLAYER_ID = "playerId";
	const std::string JSON_DIRECTION = "direction";
	const std::string JSON_POSITION = "position";
	const std::string JSON_BULLETS = "bullets";
	const std::string JSON_YAW = "yaw";
	const std::string JSON_PITCH = "pitch";
	const std::string JSON_TIME = "time";
};

