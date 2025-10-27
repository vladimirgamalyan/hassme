#pragma once
#include <string>
#include <filesystem>

class Config {
public:
	std::string mqtt_topic;   // ����� MQTT
	std::string mqtt_broker;  // ����� ������� MQTT

	// �������� ������������ �� ������������ ����
	void load();

	// �������� ������������ ���������
	static Config& getInstance();

private:
	Config() = default;                       // �������� �����������
	Config(const Config&) = delete;           // ��� �����������
	Config& operator=(const Config&) = delete; // ��� ������������
};
