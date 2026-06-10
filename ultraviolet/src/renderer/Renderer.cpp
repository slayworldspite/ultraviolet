#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "Renderer.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	Renderer::Renderer(const NativeWindow& window)
	{
		Logger::Log("initializing renderer...", Logger::Level::Info);

		m_Window = window.GetNativeHandle();
		m_GLContext = ::SDL_GL_CreateContext(m_Window);

		if (!m_GLContext)
		{
			Logger::Log("failed to create an opengl context!", Logger::Level::Critical);
			exit(-1);
		}

		if (!::SDL_GL_MakeCurrent(m_Window, static_cast<::SDL_GLContext>(m_GLContext)))
		{
			Logger::Log("failed to make the context current!", Logger::Level::Critical);
			exit(-1);
		}

		if (!::gladLoadGLLoader((GLADloadproc)::SDL_GL_GetProcAddress))
		{
			Logger::Log("failed to initialize opengl!", Logger::Level::Critical);
			exit(-1);
		}

		Logger::Log("renderer initialized!", Logger::Level::Info);

		::glEnable(GL_DEPTH_TEST);

		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SDL_GL_SetSwapInterval(1);
	}

	void Renderer::BeginFrame() const
	{
		::glClear(
			GL_COLOR_BUFFER_BIT | 
			GL_DEPTH_BUFFER_BIT
		);
	}

	void Renderer::EndFrame() const
	{
		::SDL_GL_SwapWindow(m_Window);
	}

	void Renderer::SetClearColor(const glm::vec4& color) const
	{
		::glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Draw(const Mesh& mesh, const Shader& shader) const
	{
		shader.Use();
		mesh.Bind();
		::glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.GetVertexCount()));
	}

	void Renderer::SetViewport(int width, int height) const
	{
		::glViewport(0, 0, width, height);
	}
}