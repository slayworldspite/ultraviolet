#include <format>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>

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

		m_ShadowBuffer = std::make_unique<Framebuffer>(2048, 2048, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
		m_ShadowShader = std::make_unique<Shader>("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
		m_LightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 50.0f);
	}

	Renderer::~Renderer()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplSDL3_Shutdown();
		::ImGui::DestroyContext();
	}

	void Renderer::BeginFrame() const
	{
		m_ShadowBuffer->Bind();

		::glClear(
			GL_DEPTH_BUFFER_BIT
		);

		m_ShadowBuffer->Unbind();

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
	}

	void Renderer::SetClearColor(const glm::vec4& color) const
	{
		::glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Draw(const Mesh& mesh, const Shader& shader) const
	{
		shader.Use();
		mesh.Bind();
		
		if (mesh.Material)
			mesh.Material->Apply(shader);

		auto shadowBufferSize{ m_ShadowBuffer->GetSize() };
		::glViewport(0, 0, shadowBufferSize.first, shadowBufferSize.second);

		m_ShadowBuffer->Bind();

		m_ShadowShader->Use();
		glm::mat4 lightView{ glm::lookAt(-m_LightDirection * 20.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) };
		glm::mat4 lightSpaceMatrix{ m_LightProjection * lightView };
		m_ShadowShader->Set("u_LightSpaceMatrix", lightSpaceMatrix);

		::glCullFace(GL_FRONT);
		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
		::glCullFace(GL_BACK);

		m_ShadowBuffer->Unbind();

		int width{};
		int height{};

		::SDL_GetWindowSizeInPixels(m_Window, &width, &height);
		::glViewport(0, 0, width, height);

		shader.Use();
		shader.Set("u_LightSpaceMatrix", lightSpaceMatrix);

		shader.Set("u_ShadowMap", 15);
		::glActiveTexture(GL_TEXTURE15);
		::glBindTexture(GL_TEXTURE_2D, m_ShadowBuffer->GetTexture());

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const Model& model, const Shader& shader) const
	{
		shader.Use();
		shader.Set("u_Model", model.GetModelMatrix());

		m_ShadowShader->Use();
		m_ShadowShader->Set("u_Model", model.GetModelMatrix());

		for (const Mesh& mesh : model.Meshes)
		{
			Draw(mesh, shader);
		}
	}

	void Renderer::SetViewport(int width, int height) const
	{
		::glViewport(0, 0, width, height);
	}

	void Renderer::ToggleWireframeMode() const
	{
		static bool value{ false };

		value = !value;

		// hacky, but shouldn't fail if GL_LINE = 0x1b01 and GL_FILL = 0x1b02
		::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + (!value));
	}

	void Renderer::SetVSync(bool value) const
	{
		::SDL_GL_SetSwapInterval(value);
	}

	void Renderer::SetLightDirection(float yaw, float pitch)
	{
		glm::vec3 direction{};

		direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		direction.y = sin(glm::radians(pitch));
		direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		m_LightDirection = direction;
	}
}