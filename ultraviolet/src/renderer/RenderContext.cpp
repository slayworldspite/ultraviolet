#include <cassert>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include "RenderContext.h"
#include "../utils/Logger.h"

static bool s_Initialized{ false };
static ::SDL_Window* s_Window{ nullptr };
static ::SDL_GLContext s_GLContext{};

namespace ultraviolet
{
	void RenderContext::Initialize(const NativeWindow& window)
	{
		Logger::Log("initializing render context...", Logger::Level::Info);

		assert(s_Initialized == false && "render context was initalized more than once!");

		s_Window = window.GetNativeHandle();
		s_GLContext = ::SDL_GL_CreateContext(s_Window);

		if (!s_GLContext)
		{
			Logger::Log("failed to create an opengl context!", Logger::Level::Critical);
			exit(-1);
		}

		if (!::SDL_GL_MakeCurrent(s_Window, static_cast<::SDL_GLContext>(s_GLContext)))
		{
			Logger::Log("failed to make the context current!", Logger::Level::Critical);
			exit(-1);
		}

		if (!::gladLoadGLLoader((GLADloadproc)::SDL_GL_GetProcAddress))
		{
			Logger::Log("failed to initialize opengl!", Logger::Level::Critical);
			exit(-1);
		}

		Logger::Log("render context initialized!", Logger::Level::Info);
		Logger::Log(std::format("opengl v{}", reinterpret_cast<const char*>(::glGetString(GL_VERSION))), Logger::Level::Info);

		s_Initialized = true;
	}

	::SDL_GLContext RenderContext::GetContext()
	{
		return s_GLContext;
	}

	bool RenderContext::IsInitialized()
	{
		return s_Initialized;
	}
}