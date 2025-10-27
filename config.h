#pragma once
#include <string>
#include <filesystem>

class Config {
public:
	std::string mqtt_topic;   // Топик MQTT
	std::string mqtt_broker;  // Адрес брокера MQTT

	// Загрузка конфигурации из стандартного пути
	void load();

	// Получить единственный экземпляр
	static Config& getInstance();

private:
	Config() = default;                       // Закрытый конструктор
	Config(const Config&) = delete;           // Без копирования
	Config& operator=(const Config&) = delete; // Без присваивания
};
