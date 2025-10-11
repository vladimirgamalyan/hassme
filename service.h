#pragma once
#include <windows.h>
#include <string>

DWORD WINAPI ServiceWorkerThread(LPVOID);
void StopService();
void log(const std::string& text);
void WINAPI ServiceMain(DWORD argc, LPSTR* argv);