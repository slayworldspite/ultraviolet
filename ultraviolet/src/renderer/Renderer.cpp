#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "Renderer.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	Renderer::Renderer(const NativeWindow& window)
	{
		Logger::Log("initializing renderer...", Logger::Level::Info);

		m_GLContext = SDL_GL_CreateContext(window.GetNativeHandle());

		if (!m_GLContext)
		{
			Logger::Log("failed to create an opengl context!", Logger::Level::Critical);
			exit(-1);
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			Logger::Log("failed to initialize opengl!", Logger::Level::Critical);
			exit(-1);
		}

		if (!SDL_GL_MakeCurrent(window.GetNativeHandle(), static_cast<SDL_GLContext>(m_GLContext)))
		{
			Logger::Log("failed to make the context current!", Logger::Level::Critical);
			exit(-1);
		}

		Logger::Log("renderer initialized", Logger::Level::Info);
	}
}