#pragma once

#include <stdio.h>
#include <string>
#include "json/json.h"
#include <iostream>
#include <memory>

#include "PositionData.h"
#include "DirectionData.h"
#include "CommonValues.h"
#include "BulletData.h"

using namespace std;

class PlayerDataRemote
{
public:
	string id = "";
	PositionData position;
	DirectionData direction;
	std::vector<BulletData> bullets;

private:
};

