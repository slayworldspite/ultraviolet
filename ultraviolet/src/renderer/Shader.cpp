#include <fstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) :
		m_ID{},
		m_VertexPath{ vertexPath },
		m_FragmentPath{ fragmentPath }
	{
		Load();
	}

	void Shader::Use() const
	{
		::glUseProgram(m_ID);
	}

	void Shader::Load()
	{
		Logger::Log("loading shader: " + m_VertexPath.string(), Logger::Level::Info);

		if (!std::filesystem::exists(m_VertexPath))
		{
			Logger::Log(m_VertexPath.string() + " not found!", Logger::Level::Error);
			Logger::Log("loading will continue, but the shader won't work. the errors may be erroneous!", Logger::Level::Warning);
		}

		std::string vertexSourceString{ ReadFile(m_VertexPath) };
		const char* vertexSource{ vertexSourceString.data() };

		uint32_t vertexShader{ ::glCreateShader(GL_VERTEX_SHADER) };
		::glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		::glCompileShader(vertexShader);

		LogShaderError(vertexShader);

		Logger::Log("loading shader: " + m_FragmentPath.string(), Logger::Level::Info);

		if (!std::filesystem::exists(m_FragmentPath))
		{
			Logger::Log(m_FragmentPath.string() + " not found!", Logger::Level::Error);
			Logger::Log("loading will continue, but the shader won't work. the errors may be erroneous!", Logger::Level::Warning);
		}

		std::string fragmentSourceString{ ReadFile(m_FragmentPath) };
		const char* fragmentSource{ fragmentSourceString.data() };

		uint32_t fragmentShader{ ::glCreateShader(GL_FRAGMENT_SHADER) };
		::glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		::glCompileShader(fragmentShader);

		LogShaderError(fragmentShader);

		Logger::Log("linking shaders...", Logger::Level::Info);

		m_ID = ::glCreateProgram();

		::glAttachShader(m_ID, vertexShader);
		::glAttachShader(m_ID, fragmentShader);

		::glLinkProgram(m_ID);

		int success{};

		::glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			std::string log{};
			log.resize(512);
			::glGetProgramInfoLog(m_ID, static_cast<GLsizei>(log.capacity()), nullptr, log.data());
			Logger::Log("failed to link program!", Logger::Level::Error);
			Logger::Log(log, Logger::Level::Error);
		}

		::glDeleteShader(vertexShader);
		::glDeleteShader(fragmentShader);

		Logger::Log("shaders linked!", Logger::Level::Info);
	}

	std::string Shader::ReadFile(const std::filesystem::path& path)
	{
		std::ifstream file{ path };
		std::stringstream stream;
		
		stream << file.rdbuf();
		file.close();

		return stream.str();
	}

	void Shader::LogShaderError(uint32_t shader)
	{
		int success{};

		::glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			std::string log{};
			log.resize(512);
			::glGetShaderInfoLog(shader, static_cast<GLsizei>(log.capacity()), nullptr, log.data());
			Logger::Log("failed to compile shader!", Logger::Level::Error);
			Logger::Log(log, Logger::Level::Error);
		}
	}

	int Shader::GetLocation(std::string_view name) const
	{
		return ::glGetUniformLocation(m_ID, name.data());
	}

	void Shader::Set(std::string_view name, const glm::vec3& value) const
	{
		Use();
		::glUniform3fv(GetLocation(name), 1, &value[0]);
	}

	void Shader::Set(std::string_view name, const glm::vec4& value) const
	{
		Use();
		::glUniform4fv(GetLocation(name), 1, &value[0]);
	}

	void Shader::Set(std::string_view name, const glm::mat4& value) const
	{
		Use();
		::glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::Set(std::string_view name, int value) const
	{
		Use();
		::glUniform1i(GetLocation(name), value);
	}

	void Shader::Set(std::string_view name, bool value) const
	{
		Use();
		::glUniform1i(GetLocation(name), static_cast<int>(value));
	}

	void Shader::Set(std::string_view name, float value) const
	{
		Use();
		::glUniform1f(GetLocation(name), value);
	}
}