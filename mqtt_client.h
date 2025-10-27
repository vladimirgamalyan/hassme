#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <functional>

class MqttClient {
public:
	using MessageHandler = std::function<void(const std::string& topic, const std::string& payload)>;

	MqttClient(const std::string& serverUri,
		const std::string& clientId,
		const std::string& topic,
		MessageHandler onMessage);

	void start();   // запускает внутренний поток
	void stop();    // останавливает клиент

private:
	void runLoop();

	std::string serverUri_;
	std::string clientId_;
	std::string topic_;
	MessageHandler onMessage_;

	std::atomic<bool> running_{ false };
	std::thread thread_;
};
