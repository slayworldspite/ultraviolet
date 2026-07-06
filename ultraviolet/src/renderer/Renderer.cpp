#include <format>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tracy/tracy/Tracy.hpp>

#include "Renderer.h"
#include "materials/MaterialShaderManager.h"
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

		Logger::Log(std::format("opengl v{}", reinterpret_cast<const char*>(::glGetString(GL_VERSION))), Logger::Level::Info);
		Logger::Log(std::format("glsl v{}", reinterpret_cast<const char*>(::glGetString(GL_SHADING_LANGUAGE_VERSION))), Logger::Level::Info);

		::glEnable(GL_DEPTH_TEST);

		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		::SDL_GL_SetSwapInterval(1);

		// imgui initialization
		::IMGUI_CHECKVERSION();
		::ImGui::CreateContext();

		::ImGuiIO& io{ ::ImGui::GetIO() };
		io.ConfigFlags |=
			::ImGuiConfigFlags_DockingEnable |
			::ImGuiConfigFlags_ViewportsEnable |
			::ImGuiConfigFlags_NavEnableKeyboard;

		::ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GLContext);
		::ImGui_ImplOpenGL3_Init();

		MaterialShaderManager::Initialize();
	}

	Renderer::~Renderer()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplSDL3_Shutdown();
		::ImGui::DestroyContext();
	}

	void Renderer::BeginFrame() const
	{
		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);

		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplSDL3_NewFrame();
		::ImGui::NewFrame();
	}

	void Renderer::EndFrame() const
	{
		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		::SDL_GL_SwapWindow(m_Window);

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();

		::SDL_GL_MakeCurrent(m_Window, static_cast<SDL_GLContext>(m_GLContext));

		FrameMark;
	}

	void Renderer::SetClearColor(const glm::vec4& color) const
	{
		::glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Draw(const Mesh& mesh) const
	{
		mesh.Bind();

		if (mesh.Material)
			mesh.Material->Apply();

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const Mesh& mesh, const glm::mat4& model) const
	{
		mesh.Bind();

		if (mesh.Material)
		{
			mesh.Material->Apply();
			mesh.Material->Shader->Set("u_Model", model);
		}

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const Model& model) const
	{
		for (const Mesh& mesh : model.Meshes)
		{
			Draw(mesh, model.GetModelMatrix());
		}
	}

	void Renderer::Draw(const Scene& scene) const
	{
		for (const Model& model : scene.Models)
		{
			Draw(model);
		}
	}

	void Renderer::SetViewport(int width, int height) const
	{
		::glViewport(0, 0, width, height);
	}

	void Renderer::SetWireframeMode(bool value) const
	{
		::glPolygonMode(GL_FRONT_AND_BACK, value ? GL_LINE : GL_FILL);
	}

	void Renderer::SetVSync(bool value) const
	{
		::SDL_GL_SetSwapInterval(value);
	}
}