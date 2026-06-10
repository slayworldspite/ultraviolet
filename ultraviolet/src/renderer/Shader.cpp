#include <fstream>

#include <glad/glad.h>

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
		std::string vertexSourceString{ ReadFile(m_VertexPath) };
		const char* vertexSource{ vertexSourceString.data() };

		uint32_t vertexShader{ ::glCreateShader(GL_VERTEX_SHADER) };
		::glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		::glCompileShader(vertexShader);

		LogShaderError(vertexShader);

		std::string fragmentSourceString{ ReadFile(m_FragmentPath) };
		const char* fragmentSource{ fragmentSourceString.data() };

		uint32_t fragmentShader{ ::glCreateShader(GL_FRAGMENT_SHADER) };
		::glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		::glCompileShader(fragmentShader);

		LogShaderError(fragmentShader);

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

	void Shader::Set(std::string_view name, const glm::vec4& value) const
	{
		Use();
		::glUniform4f(GetLocation(name), value.x, value.y, value.z, value.w);
	}
}