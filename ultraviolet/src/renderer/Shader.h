#pragma once

#include <filesystem>
#include <string_view>

#include <glm/glm.hpp>

namespace ultraviolet
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

		void Use() const;
		void Load();

		void Set(std::string_view name, const glm::vec2& value) const;
		void Set(std::string_view name, const glm::vec3& value) const;
		void Set(std::string_view name, const glm::vec4& value) const;
		void Set(std::string_view name, const glm::mat4& value) const;
		void Set(std::string_view name, int value) const;
		void Set(std::string_view name, bool value) const;
		void Set(std::string_view name, float value) const;

	private:
		uint32_t m_ID;
		std::filesystem::path m_VertexPath;
		std::filesystem::path m_FragmentPath;

	private:
		int GetLocation(std::string_view name) const;
		
		static std::string ReadFile(const std::filesystem::path& path);
		static void LogShaderError(uint32_t shader);
	};
}