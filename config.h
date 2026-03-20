#pragma once
#include <string>
#include <filesystem>

class Config
{
public:
	std::string mqttServer;
	std::string mqttUsername;
	std::string mqttPassword;
	std::string mqttTopic;

	// Загрузить конфигурацию из файла
	void load(const std::filesystem::path& path);

	// Глобальный доступ (Singleton)
	static Config& getInstance();

private:
	Config() = default;
	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;
};
