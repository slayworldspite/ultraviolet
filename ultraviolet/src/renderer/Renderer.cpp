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
		m_ShadowMapResolution{ 4096 },
		m_ShadowCameraSize{ 30.0f },
		m_ShadowCameraDepth{ 50.0f },
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

		InitializeFramebuffers(windowSize);
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

		m_ShadowMapFramebuffer->Bind();
		::glClear(GL_DEPTH_BUFFER_BIT);

		m_ScreenFramebuffer->Bind();

		::glEnable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);
		::glEnable(GL_CULL_FACE);
		//::glFrontFace(GL_CW);

		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);

		::glPolygonMode(GL_FRONT_AND_BACK, m_WireframeMode ? GL_LINE : GL_FILL);

		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplSDL3_NewFrame();
		::ImGui::NewFrame();
	}

	void Renderer::EndFrame() const
	{
		ZoneScoped;

		m_ScreenFramebuffer->Unbind();

		::glDisable(GL_DEPTH_TEST);
		::glDisable(GL_BLEND);
		::glDisable(GL_CULL_FACE);

		::glClear(GL_COLOR_BUFFER_BIT);

		::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		int windowWidth{};
		int windowHeight{};

		::SDL_GetWindowSizeInPixels(m_Window, &windowWidth, &windowHeight);

		::glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ScreenFramebuffer->GetID());
		::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostProcessFramebuffer->GetID());
		
		::glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		::glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_ScreenQuad->Bind();

		::glViewport(0, 0, windowWidth, windowHeight);

		PresentFrame();

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		::SDL_GL_SwapWindow(m_Window);

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();

		::SDL_GL_MakeCurrent(m_Window, static_cast<SDL_GLContext>(m_GLContext));

		FrameMark;
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		::glClearColor(color.r, color.g, color.b, color.a);
		m_ClearColor = color;
	}

	void Renderer::Draw(const Mesh& mesh) const
	{
		ZoneScoped;

		mesh.Bind();

		if (mesh.Material)
			mesh.Material->Apply();

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const Mesh& mesh, const glm::mat4& model) const
	{
		ZoneScoped;

		mesh.Bind();

		if (mesh.Material)
		{
			mesh.Material->Apply();
			mesh.Material->Shader->Set("u_Model", model);
			mesh.Material->Shader->Set("u_DepthMap", 15);
			m_ShadowMapFramebuffer->Texture.Bind(15);
		}

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Draw(const Model& model) const
	{
		ZoneScoped;

		for (const Mesh& mesh : model.Meshes)
		{
			Draw(mesh, model.GetModelMatrix());
		}
	}

	void Renderer::Draw(const Entity& entity) const
	{
		ZoneScoped;

		Draw(entity.RenderObject);
	}

	void Renderer::Draw(const Scene& scene)
	{
		ZoneScoped;

		DrawShadowMap(scene);
		
		m_ScreenFramebuffer->Bind();

		for (const Model& model : scene.Models)
		{
			if (model.Visible)
				Draw(model);
		}

		for (const Entity& entity : scene.Entities)
		{
			if (entity.RenderObject.Visible)
				Draw(entity.RenderObject);
		}
	}

	void Renderer::SetOverlay(std::shared_ptr<Texture> overlay)
	{
		m_OverlayTexture = overlay;
	}

	void Renderer::SetViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_ScreenFramebuffer = std::make_unique<Framebuffer>(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);

		m_ScreenRenderbuffer = std::make_unique<Renderbuffer>(*m_ScreenFramebuffer);
		m_ScreenRenderbuffer->Bind();
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
		InitializeFramebuffers({ width, height });
	}

	void Renderer::AddToImGui()
	{

		if (::ImGui::Button("Reload Post Processing Shaders"))
		{
			m_PostProcessingShader->Load();
		}

		::ImGui::DragFloat("Gamma", &m_Gamma, 0.05f);
		::ImGui::DragInt("Shadow Map Resolution", &m_ShadowMapResolution, 128);
		::ImGui::DragFloat("Shadow Camera Size", &m_ShadowCameraSize, 1.0f);
		::ImGui::DragFloat("Shadow Camera Depth", &m_ShadowCameraDepth, 1.0f);

		if (::ImGui::Button("Remake Shadow Map Framebuffer"))
		{
			m_ShadowMapFramebuffer = std::make_unique<Framebuffer>(m_ShadowMapResolution, m_ShadowMapResolution, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		}

		if (::ImGui::CollapsingHeader("Framebuffers"))
		{
			::ImGui::Indent(16.0f);

			::ImGui::Text("Main");
			m_PostProcessFramebuffer->AddToImGui("Main");
			::ImGui::Text("Shadow Map");
			m_ShadowMapFramebuffer->AddToImGui("Shadow Map");
			::ImGui::Text("Overlay");
			::ImGui::Image((ImTextureID)(intptr_t)m_OverlayTexture->GetID(), ImVec2{ 256.0f, 144.0f });
		}
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

	void Renderer::InitializeFramebuffers(const std::pair<int, int>& windowSize)
	{
		VertexBuffer quadVertexBuffer{ std::vector<Vertex>{
			Vertex{.Position{ -1,  1, 0 }, .TextureCoordinates{ 0.0f, 1.0f } },
			Vertex{.Position{ -1, -1, 0 }, .TextureCoordinates{ 0.0f, 0.0f } },
			Vertex{.Position{  1, -1, 0 }, .TextureCoordinates{ 1.0f, 0.0f } },
			Vertex{.Position{ -1,  1, 0 }, .TextureCoordinates{ 0.0f, 1.0f } },
			Vertex{.Position{  1, -1, 0 }, .TextureCoordinates{ 1.0f, 0.0f } },
			Vertex{.Position{  1,  1, 0 }, .TextureCoordinates{ 1.0f, 1.0f } },
		} };

		m_ScreenQuad = std::make_unique<VertexArray>(quadVertexBuffer);


		m_ScreenFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, true);
		m_ScreenRenderbuffer = std::make_unique<Renderbuffer>(*m_ScreenFramebuffer, true);

		m_PostProcessFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);

		m_ShadowMapFramebuffer = std::make_unique<Framebuffer>(m_ShadowMapResolution, m_ShadowMapResolution, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		m_ShadowMapShader = std::make_unique<Shader>("assets/shaders/ShadowMap.vert", "assets/shaders/ShadowMap.frag");
		m_ShadowMapUniformBuffer = std::make_unique<UniformBuffer>(nullptr, sizeof(DepthMapData), 4);

		m_PostProcessingShader = std::make_unique<Shader>("assets/shaders/Framebuffer.vert", "assets/shaders/PostProcessing.frag");
	}

	void Renderer::DrawShadowMap(const Scene& scene)
	{
		ZoneScoped;

		const DirectionalLight& light{ scene.DirectionalLight };

		glm::vec3 lightDirection{};

		lightDirection.x = glm::cos(glm::radians(light.Pitch)) * glm::sin(glm::radians(light.Yaw));
		lightDirection.y = glm::sin(glm::radians(light.Pitch));
		lightDirection.z = glm::cos(glm::radians(light.Pitch)) * glm::cos(glm::radians(light.Yaw));

		lightDirection = glm::normalize(lightDirection);

		glm::mat4 lightView{ glm::lookAt(-lightDirection * 25.0f, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}) };

		const glm::vec3& cameraPosition{ scene.Camera.Position };

		float texelSize{ m_ShadowCameraDepth / m_ShadowMapResolution };
		float snapFactor{ 32.0f };

		float snapSize{ texelSize * snapFactor };

		glm::vec2 snappedCameraPosition{};
		snappedCameraPosition.x = std::floor(cameraPosition.x / snapSize) * snapSize;
		snappedCameraPosition.y = std::floor(cameraPosition.z / snapSize) * snapSize;

		glm::mat4 lightProjection{ glm::ortho(
			-m_ShadowCameraSize + snappedCameraPosition.x, 
			 m_ShadowCameraSize + snappedCameraPosition.x,
			-m_ShadowCameraSize + snappedCameraPosition.y,
			 m_ShadowCameraSize + snappedCameraPosition.y,
			 1.0f, 
			 m_ShadowCameraDepth
		) };

		m_LightSpaceMatrix = lightProjection * lightView;

		m_ShadowMapUniformBuffer->SetData(&m_LightSpaceMatrix, sizeof(m_LightSpaceMatrix));

		// render
		m_ShadowMapFramebuffer->Bind();

		::glViewport(0, 0, m_ShadowMapResolution, m_ShadowMapResolution);
		::glCullFace(GL_FRONT);

		m_ShadowMapShader->Use();

		for (const Model& model : scene.Models)
		{
			if (!model.CastsShadow)
				continue;

			for (const Mesh& mesh : model.Meshes)
			{
				m_ShadowMapShader->Set("u_Model", model.GetModelMatrix());

				mesh.Bind();

				::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
			}
		}

		for (const Entity& entity : scene.Entities)
		{
			if (!entity.RenderObject.Visible || !entity.RenderObject.CastsShadow)
				continue;

			for (const Mesh& mesh : entity.RenderObject.Meshes)
			{
				m_ShadowMapShader->Set("u_Model", entity.RenderObject.GetModelMatrix());

				mesh.Bind();

				::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
			}
		}

		::glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
		::glCullFace(GL_BACK);
	}

	void Renderer::PresentFrame() const
	{
		m_PostProcessingShader->Use();

		int framebufferTextureIndex{ 15 };
		int overlayTextureIndex{ 14 };

		m_PostProcessingShader->Set("u_Framebuffer", framebufferTextureIndex);
		m_PostProcessingShader->Set("u_Overlay", overlayTextureIndex);
		m_PostProcessingShader->Set("u_HasOverlay", m_OverlayTexture != nullptr);
		m_PostProcessingShader->Set("u_Gamma", m_Gamma);

		m_PostProcessFramebuffer->Texture.Bind(framebufferTextureIndex);
		if (m_OverlayTexture)
			m_OverlayTexture->Bind(overlayTextureIndex);

		::glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}