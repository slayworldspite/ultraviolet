#include <format>
#include <random>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <tracy/tracy/Tracy.hpp>

#include "Renderer.h"
#include "materials/MaterialShaderManager.h"
#include "../utils/Logger.h"

namespace
{
	struct DepthMapData
	{
		glm::mat4 LightSpaceMatrix;
	};
}

namespace ultraviolet
{
	Renderer::Renderer(const NativeWindow& window) :
		m_WireframeMode{ false },
		m_Gamma{ 2.2f },
		m_ViewportWidth{},
		m_ViewportHeight{}
	{
		Logger::Log("initializing renderer...", Logger::Level::Info);

		CreateContext(window);

		Logger::Log("renderer initialized!", Logger::Level::Info);

		Logger::Log(std::format("opengl v{}", reinterpret_cast<const char*>(::glGetString(GL_VERSION))), Logger::Level::Info);

		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		::SDL_GL_SetSwapInterval(1);

		InitializeImGui();

		MaterialShaderManager::Initialize();

		auto windowSize{ window.GetWindowSize() };

		m_ViewportWidth = windowSize.first;
		m_ViewportHeight = windowSize.second;

		::glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
	}

	Renderer::~Renderer()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplSDL3_Shutdown();
		::ImPlot::DestroyContext();
		::ImGui::DestroyContext();
	}

	void Renderer::BeginFrame() const
	{
		ZoneScoped;

		::glEnable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);
		::glEnable(GL_CULL_FACE);

		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);

		::glPolygonMode(GL_FRONT_AND_BACK, m_WireframeMode ? GL_LINE : GL_FILL);

		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplSDL3_NewFrame();
		::ImGui::NewFrame();
	}

	void Renderer::EndFrame()
	{
		ZoneScoped;

		m_RenderQueue.Sort();
		const std::vector<DrawItem>& drawItems{ m_RenderQueue.Get() };

		for (const DrawItem& drawItem : drawItems)
		{
			Draw(drawItem);
		}

		m_RenderQueue.Clear();

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		::SDL_GL_SwapWindow(m_Window);

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();

		::SDL_GL_MakeCurrent(m_Window, static_cast<SDL_GLContext>(m_GLContext));

		FrameMark;
	}

	void Renderer::Submit(const Mesh& mesh, const glm::mat4& transform)
	{
		DrawItem drawItem{
			.Mesh = &mesh,
			.Transform = transform
		};

		drawItem.GenerateSortKey();

		m_RenderQueue.Submit(drawItem);
	}

	void Renderer::Submit(const Model& model)
	{
		const glm::mat4& transform{ model.GetModelMatrix() };

		for (const Mesh& mesh : model.Meshes)
			Submit(mesh, transform);
	}

	void Renderer::Submit(const Entity& entity)
	{
		Submit(entity.RenderObject);
	}

	void Renderer::Submit(const Scene& scene)
	{
		for (const Model& model : scene.Models)
			Submit(model);

		for (const Entity& entity : scene.Entities)
			Submit(entity);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		::glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::SetViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);

		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	void Renderer::SetWireframeMode(bool value)
	{
		m_WireframeMode = value;
	}

	void Renderer::SetVSync(bool value) const
	{
		::SDL_GL_SetSwapInterval(value);
	}

	void Renderer::Resize(int width, int height)
	{
		SetViewport(width, height);
	}

	void Renderer::AddToImGui()
	{
		::ImGui::DragFloat("Gamma", &m_Gamma, 0.05f);
	}

	void Renderer::CreateContext(const NativeWindow& window)
	{
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
	}

	void Renderer::InitializeImGui() const
	{
		::IMGUI_CHECKVERSION();
		::ImGui::CreateContext();
		
		::ImPlot::CreateContext();

		::ImGuiIO& io{ ::ImGui::GetIO() };
		io.ConfigFlags |=
			::ImGuiConfigFlags_DockingEnable |
			::ImGuiConfigFlags_ViewportsEnable |
			::ImGuiConfigFlags_NavEnableKeyboard;

		::ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GLContext);
		::ImGui_ImplOpenGL3_Init();
	}

	void Renderer::Draw(const DrawItem& drawItem) const
	{
		if (drawItem.Mesh->Material)
			drawItem.Mesh->Material->Apply();

		if (drawItem.Mesh)
		{
			drawItem.Mesh->Bind();

			::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(drawItem.Mesh->GetIndexCount()), GL_UNSIGNED_INT, nullptr);
		}
	}
}