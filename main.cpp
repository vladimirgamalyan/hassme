#include <windows.h>
#include <iostream>
#include <string>
#include "service_name.h"
#include "mqtt_client.h"
#include "config.h"
#include "log.h"
#include "volume_control.h"
#include "get_program_data_path.h"

static bool running = false;
static SERVICE_STATUS ServiceStatus = { 0 };
static SERVICE_STATUS_HANDLE hStatus = NULL;

DWORD WINAPI ServiceWorkerThread(LPVOID)
{
	running = true;

	log("Service thread started");

	auto& cfg = Config::getInstance();

	MqttClient mqtt(cfg.mqttServer, SERVICE_NAME, cfg.mqttTopic,
		cfg.mqttUsername, cfg.mqttPassword,

		[](const std::string& topic, const std::string& payload) {
			auto p = payload;
			std::transform(p.begin(), p.end(), p.begin(), ::tolower);
			if (topic == "homeassistant/pc/mute") {
				if (p == "toggle") muteAudio(!isAudioMuted());
				else if (p == "1" || p == "on" || p == "true")  muteAudio(true);
				else if (p == "0" || p == "off" || p == "false") muteAudio(false);
			}
		}
	);

	mqtt.start();

	while (running)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	mqtt.stop();

	log("Service thread exiting");
	return 0;
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

void WINAPI ServiceMain(DWORD, LPSTR*)
{
	hStatus = RegisterServiceCtrlHandlerA(SERVICE_NAME, ServiceCtrlHandler);
	if (!hStatus)
		return;

	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	SetServiceStatus(hStatus, &ServiceStatus);

	CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);

	log("Service started");
}

void runConsoleMode()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
	freopen_s(&stream, "CONIN$", "r", stdin);

	std::cout << "=== " << SERVICE_NAME << " console mode ===\n";
	std::cout << "Press Ctrl+C to stop.\n";

	ServiceWorkerThread(nullptr);

	std::cout << "Service stopped.\n";
	FreeConsole();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdLine, int)
{
	auto configPath = getProgramDataPath() / SERVICE_NAME / "config.json";
	Config::getInstance().load(configPath);

	SERVICE_TABLE_ENTRYA ServiceTable[] =
	{
		{ (LPSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTIONA)ServiceMain },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcherA(ServiceTable))
	{
		DWORD err = GetLastError();
		if (err == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
		{
			runConsoleMode();
		}
		else
		{
			char msg[256];
			sprintf_s(msg, "StartServiceCtrlDispatcher failed with error code %lu", err);
			MessageBoxA(NULL, msg, SERVICE_NAME, MB_ICONERROR | MB_OK);
		}
	}

	return 0;
}
