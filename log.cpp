#include "log.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <iostream>
#include <windows.h>
#include "get_program_data_path.h"
#include "service_name.h"

//TODO: удалять старые логи

namespace fs = std::filesystem;

static const size_t MAX_LOG_SIZE = 1 * 1024 * 1024; // 1 МБ

// Проверка и ротация при превышении размера
static void rotateLogIfNeeded()
{
	try
	{
		auto baseDir = getProgramDataPath() / SERVICE_NAME;

		std::filesystem::path log_file_path = baseDir / "service.log";

		if (fs::exists(log_file_path))
		{
			auto size = fs::file_size(log_file_path);
			if (size > MAX_LOG_SIZE)
			{
				auto now = std::chrono::system_clock::now();
				auto t = std::chrono::system_clock::to_time_t(now);
				std::tm tm{};
				localtime_s(&tm, &t);

				char newName[128];
				sprintf_s(newName, "service_%04d%02d%02d_%02d%02d%02d.log",
					tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
					tm.tm_hour, tm.tm_min, tm.tm_sec);

				fs::rename(log_file_path, baseDir / newName);
			}
		}
	}
	catch (...)
	{
		// Игнорируем любые ошибки при ротации
	}
}

void log(const std::string& text)
{
	try
	{
		auto baseDir = getProgramDataPath() / SERVICE_NAME;
		std::filesystem::create_directories(baseDir);
		std::filesystem::path log_file_path = baseDir / "service.log";

		rotateLogIfNeeded();

		std::ofstream f(log_file_path, std::ios::app);
		if (!f.is_open())
			return;

		// Время и дата
		auto now = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(now);
		std::tm tm{};
		localtime_s(&tm, &t);

		f << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "  " << text << "\n";
		f.close();

		if (GetConsoleWindow())
			std::cout << std::put_time(&tm, "%H:%M:%S") << "  " << text << std::endl;
	}
	catch (...)
	{
		// Логирование не должно ломать сервис
	}
}
