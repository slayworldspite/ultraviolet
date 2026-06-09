#pragma once

#include <filesystem>

namespace ultraviolet
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

		void Use() const;
		void Load();

	private:
		uint32_t m_ID;
		std::filesystem::path m_VertexPath;
		std::filesystem::path m_FragmentPath;

	private:
		static std::string ReadFile(const std::filesystem::path& path);
		static void LogShaderError(uint32_t shader);
	};
}