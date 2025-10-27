#include <windows.h>
#include <mqtt/async_client.h>
#include "mqtt_client.h"
#include "log.h"

using namespace std::chrono_literals;

MqttClient::MqttClient(const std::string& serverUri,
	const std::string& clientId,
	const std::string& topic,
	MessageHandler onMessage)
	: serverUri_(serverUri), clientId_(clientId), topic_(topic), onMessage_(onMessage)
{
}

void MqttClient::start()
{
	running_ = true;
	thread_ = std::thread(&MqttClient::runLoop, this);
}

void MqttClient::stop()
{
	running_ = false;
	if (thread_.joinable())
		thread_.join();
}

void MqttClient::runLoop()
{
	mqtt::async_client client(serverUri_, clientId_);
	mqtt::connect_options connOpts;
	connOpts.set_clean_session(true);
	connOpts.set_user_name("Moskito");
	connOpts.set_password("supEROass42!2");

	// Обработка сообщений
	client.set_message_callback([this](mqtt::const_message_ptr msg) {
		if (onMessage_) {
			onMessage_(msg->get_topic(), msg->to_string());
		}
		});

	while (running_)
	{
		try {
			if (!client.is_connected()) 
			{
				log("Connecting to MQTT broker: " + serverUri_);
				client.connect(connOpts)->wait();
				log("Connected to broker, subscribing to " + topic_);

				client.subscribe(topic_, 1)->wait();
				log("Subscribed successfully");
			}

			// Периодический опрос состояния
			std::this_thread::sleep_for(1s);
		}
		catch (const mqtt::exception& e) 
		{
			log(std::string("MQTT error: ") + e.what());
			try { client.disconnect()->wait(); }
			catch (...) {}
			log("Reconnecting in 5 seconds...");
			std::this_thread::sleep_for(5s);
		}
	}

	// При завершении
	try {
		if (client.is_connected()) {
			log("Disconnecting from broker...");
			client.unsubscribe(topic_)->wait();
			client.disconnect()->wait();
		}
	}
	catch (...) {}
	log("MQTT loop exited");
}
