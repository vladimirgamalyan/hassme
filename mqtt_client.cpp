#include <windows.h>
#include <mqtt/async_client.h>
#include "mqtt_client.h"
#include "log.h"

using namespace std::chrono_literals;

MqttClient::MqttClient(const std::string& serverUri,
	const std::string& clientId,
	const std::string& topic,
	const std::string& username,
	const std::string& password,
	MessageHandler onMessage)
	: serverUri_(serverUri),
	clientId_(clientId),
	topic_(topic),
	username_(username),
	password_(password),
	onMessage_(onMessage)
{
}

void MqttClient::start()
{
	if (running_) return;
	running_ = true;
	thread_ = std::thread(&MqttClient::runLoop, this);
}

void MqttClient::stop()
{
	running_ = false;
	if (thread_.joinable())
		thread_.join();
}

void MqttClient::publish(const std::string& topic, const std::string& payload, int qos, bool retained)
{
	std::lock_guard<std::mutex> lock(clientMutex_);
	if (client_ && client_->is_connected()) {
		try {
			auto msg = mqtt::make_message(topic, payload);
			msg->set_qos(qos);
			msg->set_retained(retained);
			client_->publish(msg);
		}
		catch (const mqtt::exception& e) {
			log(std::string("MQTT publish failed: ") + e.what());
		}
	}
	else {
		log("MQTT publish skipped (not connected)");
	}
}

void MqttClient::runLoop()
{
	client_ = std::make_shared<mqtt::async_client>(serverUri_, clientId_);
	mqtt::connect_options connOpts;
	connOpts.set_clean_session(true);
	if (!username_.empty())
		connOpts.set_user_name(username_);
	if (!password_.empty())
		connOpts.set_password(password_);

	client_->set_message_callback([this](mqtt::const_message_ptr msg) {
		if (onMessage_) {
			onMessage_(msg->get_topic(), msg->to_string());
		}
		});

	while (running_) {
		try {
			if (!client_->is_connected()) {
				log("Connecting to MQTT broker: " + serverUri_);
				client_->connect(connOpts)->wait();
				log("Connected to broker, subscribing to " + topic_);
				client_->subscribe(topic_, 1)->wait();
				log("Subscribed successfully");
			}
			std::this_thread::sleep_for(1s);
		}
		catch (const mqtt::exception& e) {
			log(std::string("MQTT error: ") + e.what());
			try { client_->disconnect()->wait(); }
			catch (...) {}
			log("Reconnecting in 5 seconds...");
			std::this_thread::sleep_for(5s);
		}
	}

	try {
		if (client_->is_connected()) {
			log("Disconnecting from broker...");
			client_->unsubscribe(topic_)->wait();
			client_->disconnect()->wait();
		}
	}
	catch (...) {}

	client_.reset();
	log("MQTT loop exited");
}
