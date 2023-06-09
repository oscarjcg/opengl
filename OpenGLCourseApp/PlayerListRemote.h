#pragma once
#include <vector>

#include <stdio.h>
#include <string>
#include "json/json.h"
#include <iostream>
#include <memory>

#include "PositionData.h"
#include "DirectionData.h"
#include "CommonValues.h"
#include "PlayerDataRemote.h"

using namespace std;

class PlayerListRemote
{
public:
	std::vector<PlayerDataRemote*> playerListRemote;

	int Parse(string json);

private:
	int ParseJson(string rawJson, Json::Value& root);

	const std::string JSON_ID = "id";
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

