#include <windows.h>
#include <iostream>
#include <string>
#include "service.h"

const char* SERVICE_NAME = "WinMqttService";

bool installService(const std::string& path)
{
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!scm) return false;

	SC_HANDLE service = CreateServiceA(
		scm, SERVICE_NAME, SERVICE_NAME,
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		path.c_str(), NULL, NULL, NULL, NULL, NULL);

	if (!service)
	{
		CloseServiceHandle(scm);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return true;
}

bool uninstallService()
{
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!scm) return false;

	SC_HANDLE service = OpenServiceA(scm, SERVICE_NAME, DELETE);
	if (!service)
	{
		CloseServiceHandle(scm);
		return false;
	}

	bool ok = DeleteService(service);
	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return ok;
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

	std::string args = GetCommandLineA();
	if (args.find("install") != std::string::npos)
	{
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		if (installService(path))
			MessageBoxA(NULL, "Service installed successfully", SERVICE_NAME, MB_OK);
		else
			MessageBoxA(NULL, "Failed to install service", SERVICE_NAME, MB_ICONERROR);
		return 0;
	}
	else if (args.find("uninstall") != std::string::npos)
	{
		if (uninstallService())
			MessageBoxA(NULL, "Service removed successfully", SERVICE_NAME, MB_OK);
		else
			MessageBoxA(NULL, "Failed to remove service", SERVICE_NAME, MB_ICONERROR);
		return 0;
	}

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
