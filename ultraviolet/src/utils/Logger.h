#pragma once

#include <string_view>
#include <fstream>
#include <filesystem>

namespace ultraviolet
{
	class Logger
	{
	public:
		enum class Level
		{
			Info,
			Warning,
			Error,
			Critical,
			Debug
		};

		static void Initialize(const std::filesystem::path& directory);
		static void Log(std::string_view text, Level level);
		static void Close();

	private:
		static bool s_Initialized;
		static std::ofstream s_File;
	};
}