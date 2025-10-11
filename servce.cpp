#include "service.h"
//#include "volume_control.h"
//#include "config.h"
#include "log.h"
//#include "mqtt_stub.h"

#include <thread>
#include <chrono>

static bool running = false;
static SERVICE_STATUS ServiceStatus = { 0 };
static SERVICE_STATUS_HANDLE hStatus = NULL;

void WINAPI ServiceMain(DWORD argc, LPSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);

DWORD WINAPI ServiceWorkerThread(LPVOID)
{
	running = true;

	log("Service thread started");

	//Config cfg("C:\\ProgramData\\WinMqttService\\config.json");
	//MqttStub mqtt(cfg.mqtt_topic);

	//mqtt.setCallback([](const std::string& msg) {
	//	log("Received: " + msg);
	//	if (msg == "on") muteAudio(true);
	//	else if (msg == "off") muteAudio(false);
	//	});

	while (running)
	{
		//mqtt.loop();
		//cfg.checkForUpdates();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	log("Service thread exiting");
	return 0;
}

void WINAPI ServiceMain(DWORD, LPSTR*)
{
	hStatus = RegisterServiceCtrlHandlerA("WinMqttService", ServiceCtrlHandler);
	if (!hStatus)
		return;

	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	SetServiceStatus(hStatus, &ServiceStatus);

	// Запускаем основной рабочий поток
	//running = true;
	CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);

	log("Service started");
}

void WINAPI ServiceCtrlHandler(DWORD ctrlCode)
{
	switch (ctrlCode)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		log("Service stop requested");

		ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(hStatus, &ServiceStatus);

		running = false;

		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);

		log("Service stopped");
		break;

	default:
		break;
	}
}

void StopService()
{
	log("StopService() called");
	running = false;
}
