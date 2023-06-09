#include "JsonUtils.h"

string JsonUtils::getFieldJson(string key, string value) {
	return "\"" + key + "\":\"" + value + "\"";
}

string JsonUtils::getJsonObject(std::vector<string> fields) {
	string json = "{";

	for (int i = 0; i < fields.size(); i++) {
		json += fields[i];

		if (i != fields.size() - 1)
			json += ",";
	}

	json += "}";
	return json;
}

string JsonUtils::getCameraPositionJson(Camera camera, string time, string playerId) {
	string x = to_string(camera.getCameraPosition().x);
	string y = to_string(camera.getCameraPosition().y);
	string z = to_string(camera.getCameraPosition().z);

	string xField = getFieldJson(JSON_X, x);
	string yField = getFieldJson(JSON_Y, y);
	string zField = getFieldJson(JSON_Z, z);
	string playerIdField = getFieldJson(JSON_PLAYER_ID, playerId);
	string timeField = getFieldJson(JSON_TIME, time);

	std::vector<string> fields;
	fields.push_back(xField);
	fields.push_back(yField);
	fields.push_back(zField);
	fields.push_back(playerIdField);
	fields.push_back(timeField);

	return getJsonObject(fields);
}

string JsonUtils::getCameraDirectionJson(Camera camera) {
	string x = to_string(camera.getCameraDirection().x);
	string y = to_string(camera.getCameraDirection().y);
	string z = to_string(camera.getCameraDirection().z);
	string yaw = to_string(camera.getYaw());
	string pitch = to_string(camera.getPitch());
	string xField = getFieldJson(JSON_X, x);
	string yField = getFieldJson(JSON_Y, y);
	string zField = getFieldJson(JSON_Z, z);
	string yawField = getFieldJson(JSON_YAW, yaw);
	string fieldField = getFieldJson(JSON_PITCH, pitch);

	std::vector<string> fields;
	fields.push_back(xField);
	fields.push_back(yField);
	fields.push_back(zField);
	fields.push_back(yawField);
	fields.push_back(fieldField);

	return getJsonObject(fields);
}

string JsonUtils::getBulletJson(Model* model) {
	string x = to_string(model->GetPosition().x);
	string y = to_string(model->GetPosition().y);
	string z = to_string(model->GetPosition().z);
	string yaw = to_string(model->GetYaw());
	string pitch = to_string(model->GetPitch());
	string xField = getFieldJson(JSON_X, x);
	string yField = getFieldJson(JSON_Y, y);
	string zField = getFieldJson(JSON_Z, z);
	string yawField = getFieldJson(JSON_YAW, yaw);
	string fieldField = getFieldJson(JSON_PITCH, pitch);

	std::vector<string> fields;
	fields.push_back(xField);
	fields.push_back(yField);
	fields.push_back(zField);
	fields.push_back(yawField);
	fields.push_back(fieldField);

	return getJsonObject(fields);
}

string JsonUtils::getBulletsJson(std::vector<Model*> models) {

	std::vector<string> fields;
	for (size_t i = 0; i < models.size(); i++)
	{
		if (MODEL_BULLET_LOCAL == models[i]->GetType()) {
			string bulletJson = getBulletJson(models[i]);
			string bullet = getFieldJson(to_string(i), bulletJson);
			fields.push_back(bullet);
		}
	}

	return getJsonObject(fields);
}

string JsonUtils::getNewPositionJson(std::vector<Model*> models, Camera camera, string time, string playerId)
{
	string positionJson = getCameraPositionJson(camera, time, playerId);

	string positionField = getFieldJson(JSON_POSITION, positionJson);
	string directionField = getFieldJson(JSON_DIRECTION, getCameraDirectionJson(camera));
	string bulletsField = getFieldJson(JSON_BULLETS, getBulletsJson(models));
	std::vector<string> fields;
	fields.push_back(positionField);
	fields.push_back(directionField);
	fields.push_back(bulletsField);

	return getJsonObject(fields);
}