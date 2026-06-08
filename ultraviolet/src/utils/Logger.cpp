#include <iostream>
#include <chrono>

#include "Logger.h"

namespace ultraviolet
{
	bool Logger::s_Initialized{ false };
	std::ofstream Logger::s_File{};

	void Logger::Initialize(const std::filesystem::path& directory)
	{
		std::filesystem::path fullPath{ directory / "latest.log" };

		if (!std::filesystem::exists(directory))
		{
			std::filesystem::create_directory(directory);
		}

		if (std::filesystem::exists(fullPath))
		{
			auto lastWriteTime{ std::chrono::floor<std::chrono::seconds>(std::filesystem::last_write_time(fullPath)) };

			std::string name{ std::format("{:%F-%H-%M-%S}.log", lastWriteTime) };

			std::filesystem::rename(fullPath, directory / name);
		}

		s_File = std::ofstream{ fullPath, std::ios::out };

		s_Initialized = true;
	}

	void Logger::Log(std::string_view text, Level level)
	{
		if (!s_Initialized)
			return;

#ifdef NDEBUG
		if (level == Level::Debug)
			return;
#endif // NDEBUG

		auto now{ std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()) };

		std::string timestamp{ std::format("{:%T}", now) };

		std::cout << "\x1B[";

		switch (level)
		{
		case Level::Debug:
			std::cout << "34m[" + timestamp + " DEBUG] ";
			s_File << "[" + timestamp + " DEBUG] ";
			break;

		case Level::Info:
			std::cout << "37m[" + timestamp + " INFO] ";
			s_File << "[" + timestamp + " INFO] ";
			break;

		case Level::Warning:
			std::cout << "33m[" + timestamp + " WARNING] ";
			s_File << "[" + timestamp + " WARNING] ";
			break;

		case Level::Error:
			std::cout << "31m[" + timestamp + " ERROR] ";
			s_File << "[" + timestamp + " ERROR] ";
			break;

		case Level::Critical:
			std::cout << "5;35m--- [" + timestamp + " CRITICAL] ";
			s_File << "--- [" + timestamp + " CRITICAL] ";
			break;
		}

		std::cout << text << "\x1B[0m" << "\n";
		s_File << text << "\n";
		s_File.flush();
	}

	void Logger::Close()
	{
		s_File.close();

		s_Initialized = false;
	}
}