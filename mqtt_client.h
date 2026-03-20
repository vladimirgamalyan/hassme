#pragma once
#include <string>
#include <atomic>
#include <thread>
#include <functional>
#include <memory>
#include <mutex>

namespace mqtt {
	class async_client;
}

class MqttClient {
public:
	using MessageHandler = std::function<void(const std::string& topic, const std::string& payload)>;

	MqttClient(const std::string& serverUri,
		const std::string& clientId,
		const std::string& topic,
		const std::string& username,
		const std::string& password,
		MessageHandler onMessage);

	void start();
	void stop();
	void publish(const std::string& topic, const std::string& payload, int qos = 1, bool retained = false);

private:
	void runLoop();

	std::string serverUri_;
	std::string clientId_;
	std::string topic_;
	std::string username_;
	std::string password_;
	MessageHandler onMessage_;

	std::atomic<bool> running_{ false };
	std::thread thread_;
	std::shared_ptr<class mqtt::async_client> client_;
	std::mutex clientMutex_;
};
