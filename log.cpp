#include "log.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include <windows.h>

void log(const std::string& text)
{
	try
	{
		// ��������, ��� ������� ����������
		std::filesystem::create_directories("C:\\ProgramData\\WinMqttService");

		// ��������� ��� � ������ ����������
		std::ofstream f("C:\\ProgramData\\WinMqttService\\service.log", std::ios::app);
		if (!f.is_open())
			return;

		// ����������� ������� ���� � �����
		auto now = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(now);
		std::tm tm{};
		localtime_s(&tm, &t);

		f << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "  " << text << "\n";
		f.close();

		// ���� ���� ������� � ��������� ����
		if (GetConsoleWindow())
		{
			std::cout << std::put_time(&tm, "%H:%M:%S") << "  " << text << std::endl;
		}
	}
	catch (...)
	{
		// ����������� �� ������ ������ ���� �� ����
	}
}
