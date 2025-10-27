#include "config.h"
#include "log.h"
#include "get_program_data_path.h"
#include "service_name.h"
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

Config& Config::getInstance()
{
	static Config instance;
	return instance;
}

void Config::load()
{
	auto configPath = getProgramDataPath() / SERVICE_NAME / "config.json";

	std::ifstream f(configPath);

	json j;
	f >> j;

	mqtt_topic = j.at("mqtt_topic").get<std::string>();
	mqtt_broker = j.at("mqtt_broker").get<std::string>();
}
