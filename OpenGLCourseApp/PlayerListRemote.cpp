#include "PlayerListRemote.h"

int PlayerListRemote::Parse(string json)
{
	Json::Value root;
	if (ParseJson(json, root) != 0) {
		return 1;
	}

	std::cout << "size " << endl;
	std::cout << root.size();


	Json::Value::Members members = root.getMemberNames();
	for (int i = 0; i < members.size(); i++)
	{
		try {
			Json::Value rootPlayer = root[members[i]];
			//cout << "root " << rootPlayer[JSON_ID] << endl;
			//cout << "root " << rootPlayer[JSON_POSITION][JSON_PLAYER_ID] << endl;

			string playerId = rootPlayer[JSON_ID].asString();

			Json::Value rootPosition = rootPlayer[JSON_POSITION];
			PositionData positionData;
			positionData.x = std::stof(rootPosition[JSON_X].asString());
			positionData.y = std::stof(rootPosition[JSON_Y].asString());
			positionData.z = std::stof(rootPosition[JSON_Z].asString());

			Json::Value rootDirection = rootPlayer[JSON_DIRECTION];
			DirectionData directionData;
			directionData.playerId = playerId;
			directionData.x = std::stof(rootDirection[JSON_X].asString());
			directionData.y = std::stof(rootDirection[JSON_Y].asString());
			directionData.z = std::stof(rootDirection[JSON_Z].asString());
			directionData.yaw = std::stof(rootDirection[JSON_YAW].asString());

			PlayerDataRemote* playerDataRemote = new PlayerDataRemote();
			playerDataRemote->id = playerId;
			playerDataRemote->position = positionData;
			playerDataRemote->direction = directionData;

			Json::Value rootBullets = rootPlayer[JSON_BULLETS];
			Json::Value::Members bulletMembers = rootBullets.getMemberNames();
			for (int j = 0; j < bulletMembers.size(); j++)
			{
				Json::Value bullet = rootBullets[bulletMembers[j]];
				BulletData bulletData;
				bulletData.x = std::stof(bullet[JSON_X].asString());
				bulletData.y = std::stof(bullet[JSON_Y].asString());
				bulletData.z = std::stof(bullet[JSON_Z].asString());
				playerDataRemote->bullets.push_back(bulletData);
			}

			playerListRemote.push_back(playerDataRemote);
		}
		catch (exception) {
			cout << "PlayerListRemote exception Parse" << endl;
		}
	}

	return 0;
}

int PlayerListRemote::ParseJson(string rawJson, Json::Value& root) {
	JSONCPP_STRING err;
	const auto rawJsonLength = static_cast<int>(rawJson.length());
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err)) {
		std::cout << "error ParseJson" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "sucess " << "ParseJson" << std::endl;

	return 0;
}

