#include "config.h"
#include "log.h"
#include <fstream>
#include "nlohmann/json.hpp"
#include <windows.h>
#include <shlobj.h>

using json = nlohmann::json;

Config& Config::getInstance()
{
	static Config instance;
	return instance;
}

void Config::load(const std::filesystem::path& path)
{
	std::ifstream f(path);
	if (!f.is_open()) {
		log("Config: cannot open file " + path.string());
		return;
	}

	try {
		json j;
		f >> j;

		if (j.contains("mqtt")) {
			auto& m = j["mqtt"];
			mqttServer = m.value("server", "tcp://localhost:1883");
			mqttUsername = m.value("username", "");
			mqttPassword = m.value("password", "");
			mqttTopic = m.value("topic", "homeassistant/pc/#");
		}
		else {
			log("Config: missing 'mqtt' section");
		}

		log("Config loaded successfully: " + mqttServer);
	}
	catch (const std::exception& e) {
		log(std::string("Config parse error: ") + e.what());
	}
}
