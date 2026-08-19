#include <format>
#include <random>

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
		m_EnableBloom{ true },
		m_EnableSSAO{ true },
		m_Gamma{ 2.2f },
		m_BloomStrength{ 1.0f },
		m_SSAOStrength{ 0.75f },
		m_ViewportWidth{},
		m_ViewportHeight{}
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

		SDL_GetWindowSizeInPixels(m_Window, &m_ViewportWidth, &m_ViewportHeight);

		::glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);

		VertexBuffer quadVertexBuffer{ std::vector<Vertex>{
			Vertex{.Position{ -1,  1, 0 }, .TextureCoordinates{ 0.0f, 1.0f } },
			Vertex{.Position{ -1, -1, 0 }, .TextureCoordinates{ 0.0f, 0.0f } },
			Vertex{.Position{  1, -1, 0 }, .TextureCoordinates{ 1.0f, 0.0f } },
			Vertex{.Position{ -1,  1, 0 }, .TextureCoordinates{ 0.0f, 1.0f } },
			Vertex{.Position{  1, -1, 0 }, .TextureCoordinates{ 1.0f, 0.0f } },
			Vertex{.Position{  1,  1, 0 }, .TextureCoordinates{ 1.0f, 1.0f } },
		} };

		m_ScreenQuad = std::make_unique<VertexArray>(quadVertexBuffer);

		auto windowSize{ window.GetWindowSize() };

		m_ScreenFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, true);
		m_ScreenRenderbuffer = std::make_unique<Renderbuffer>(*m_ScreenFramebuffer, true);
		
		m_PostProcessFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);

		m_ShadowMapFramebuffer = std::make_unique<Framebuffer>(4096, 4096, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		m_ShadowMapShader = std::make_unique<Shader>("assets/shaders/ShadowMap.vert", "assets/shaders/ShadowMap.frag");
		m_ShadowMapUniformBuffer = std::make_unique<UniformBuffer>(nullptr, sizeof(DepthMapData), 4);

		m_NormalMapFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_NormalMapRenderbuffer = std::make_unique<Renderbuffer>(*m_NormalMapFramebuffer);
		m_NormalMapShader = std::make_unique<Shader>("assets/shaders/NormalMap.vert", "assets/shaders/NormalMap.frag");

		m_DepthMapFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		m_DepthMapShader = std::make_unique<Shader>("assets/shaders/DepthMap.vert", "assets/shaders/DepthMap.frag");

		m_SSAOFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_SSAOShader = std::make_unique<Shader>("assets/shaders/Framebuffer.vert", "assets/shaders/SSAO.frag");

		m_SSAOHorizontalBlurFramebuffer = std::make_unique<Framebuffer>(windowSize.first / 2, windowSize.second / 2, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_SSAOVerticalBlurFramebuffer = std::make_unique<Framebuffer>(windowSize.first / 2, windowSize.second / 2, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		
		m_BrightExtractFramebuffer = std::make_unique<Framebuffer>(windowSize.first, windowSize.second, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_BrightExtractShader = std::make_unique<Shader>("assets/shaders/Framebuffer.vert", "assets/shaders/BrightExtract.frag");

		m_HorizontalBlurFramebuffer = std::make_unique<Framebuffer>(windowSize.first / 2, windowSize.second / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_VerticalBlurFramebuffer = std::make_unique<Framebuffer>(windowSize.first / 2, windowSize.second / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);

		m_BlurShader = std::make_unique<Shader>("assets/shaders/Framebuffer.vert", "assets/shaders/Blur.frag");

		m_PostProcessingShader = std::make_unique<Shader>("assets/shaders/Framebuffer.vert", "assets/shaders/PostProcessing.frag");
	}

	Renderer::~Renderer()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplSDL3_Shutdown();
		::ImGui::DestroyContext();
	}

	void Renderer::BeginFrame() const
	{
		ZoneScoped;

		m_ShadowMapFramebuffer->Bind();
		::glClear(GL_DEPTH_BUFFER_BIT);
		m_ShadowMapFramebuffer->Unbind();

		m_NormalMapFramebuffer->Bind();
		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);
		m_NormalMapFramebuffer->Unbind();

		m_DepthMapFramebuffer->Bind();
		::glClear(GL_DEPTH_BUFFER_BIT);
		m_DepthMapFramebuffer->Unbind();

		m_ScreenFramebuffer->Bind();

		::glEnable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);
		::glEnable(GL_CULL_FACE);
		::glFrontFace(GL_CW);

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

		if (m_EnableBloom)
		{
			// bright extract
			m_BrightExtractFramebuffer->Bind();
			m_BrightExtractShader->Use();
			m_PostProcessFramebuffer->Texture.Bind();

			m_ScreenQuad->Bind();

			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_BrightExtractFramebuffer->Unbind();

			// horizontal blur
			m_PostProcessFramebuffer->Texture.Unbind();
			m_BrightExtractFramebuffer->Texture.Bind();
			m_HorizontalBlurFramebuffer->Bind();
			m_BlurShader->Use();

			m_BlurShader->Set("u_Resolution", glm::vec2{ windowWidth / 2, windowHeight / 2 });
			m_BlurShader->Set("u_Direction", glm::vec2{ 1.0f, 0.0f });

			::glViewport(0, 0, windowWidth / 2, windowHeight / 2);
			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_HorizontalBlurFramebuffer->Unbind();

			// vertical blur
			m_BrightExtractFramebuffer->Texture.Unbind();
			m_HorizontalBlurFramebuffer->Texture.Bind();

			m_VerticalBlurFramebuffer->Bind();
			m_BlurShader->Set("u_Direction", glm::vec2{ 0.0f, 1.0f });

			::glViewport(0, 0, windowWidth / 2, windowHeight / 2);
			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_VerticalBlurFramebuffer->Unbind();
			m_VerticalBlurFramebuffer->Texture.Unbind();
			m_HorizontalBlurFramebuffer->Texture.Unbind();
		}
		
		if (m_EnableSSAO)
		{
			// ssao
			m_SSAOFramebuffer->Bind();
			m_SSAOShader->Set("u_NormalMap", 15);
			m_SSAOShader->Set("u_DepthMap", 14);
			m_NormalMapFramebuffer->Texture.Bind(15);
			m_DepthMapFramebuffer->Texture.Bind(14);

			::glViewport(0, 0, windowWidth, windowHeight);
			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_NormalMapFramebuffer->Texture.Unbind();
			m_DepthMapFramebuffer->Texture.Unbind();
			m_SSAOFramebuffer->Unbind();

			// blur ssao

			m_SSAOFramebuffer->Texture.Bind();
			m_SSAOHorizontalBlurFramebuffer->Bind();
			m_BlurShader->Use();

			m_BlurShader->Set("u_Resolution", glm::vec2{ windowWidth / 2, windowHeight / 2 });
			m_BlurShader->Set("u_Direction", glm::vec2{ 1.0f, 0.0f });

			::glViewport(0, 0, windowWidth / 2, windowHeight / 2);
			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_SSAOHorizontalBlurFramebuffer->Unbind();
			m_SSAOFramebuffer->Texture.Unbind();

			m_SSAOHorizontalBlurFramebuffer->Texture.Bind();
			m_SSAOVerticalBlurFramebuffer->Bind();

			m_BlurShader->Set("u_Resolution", glm::vec2{ windowWidth / 2, windowHeight / 2 });
			m_BlurShader->Set("u_Direction", glm::vec2{ 0.0f, 1.0f });

			::glViewport(0, 0, windowWidth / 2, windowHeight / 2);
			::glDrawArrays(GL_TRIANGLES, 0, 6);

			m_SSAOHorizontalBlurFramebuffer->Texture.Unbind();
			m_SSAOVerticalBlurFramebuffer->Unbind();
		}

		::glViewport(0, 0, windowWidth, windowHeight);

		// draw to screen
		m_PostProcessingShader->Use();
			
		m_PostProcessingShader->Set("u_Framebuffer", 15);
		if (m_EnableBloom)
			m_PostProcessingShader->Set("u_Blur", 14);
		m_PostProcessingShader->Set("u_Overlay", 13);
		m_PostProcessingShader->Set("u_NormalMap", 12);
		m_PostProcessingShader->Set("u_SSAO", 11);
		m_PostProcessingShader->Set("u_EnableBloom", m_EnableBloom);
		m_PostProcessingShader->Set("u_EnableSSAO", m_EnableSSAO);
		m_PostProcessingShader->Set("u_Gamma", m_Gamma);

		m_PostProcessFramebuffer->Texture.Bind(15);
		m_OverlayTexture->Bind(13);

		::glDrawArrays(GL_TRIANGLES, 0, 6);

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

	void Renderer::Draw(const Scene& scene)
	{
		// shadow map preparation
		const DirectionalLight& light{ scene.DirectionalLight };

		glm::vec3 lightDirection{};

		lightDirection.x = glm::cos(glm::radians(light.Pitch)) * glm::sin(glm::radians(light.Yaw));
		lightDirection.y = glm::sin(glm::radians(light.Pitch));
		lightDirection.z = glm::cos(glm::radians(light.Pitch)) * glm::cos(glm::radians(light.Yaw));
		
		lightDirection = glm::normalize(lightDirection);

		glm::mat4 lightView{ glm::lookAt(-lightDirection * 25.0f, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}) };

		glm::mat4 lightProjection{ glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 50.0f) };

		m_LightSpaceMatrix = lightProjection * lightView;

		m_ShadowMapUniformBuffer->SetData(&m_LightSpaceMatrix, sizeof(m_LightSpaceMatrix));

		// shadow map
		m_ScreenFramebuffer->Unbind();
		m_ShadowMapFramebuffer->Bind();

		::glViewport(0, 0, 4096, 4096);
		::glCullFace(GL_FRONT);

		m_ShadowMapShader->Use();

		for (const Model& model : scene.Models)
		{
			if (!model.Visible || !model.CastsShadow)
				continue;

			for (const Mesh& mesh : model.Meshes)
			{
				m_ShadowMapShader->Set("u_Model", model.GetModelMatrix());

				mesh.Bind();

				::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
			}
		}
		
		m_ShadowMapFramebuffer->Unbind();

		::glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
		::glCullFace(GL_BACK);

		if (m_EnableSSAO)
		{
			// normal map
			m_NormalMapFramebuffer->Bind();
			m_NormalMapShader->Use();

			for (const Model& model : scene.Models)
			{
				if (!model.Visible || !model.CastsShadow)
					continue;

				for (const Mesh& mesh : model.Meshes)
				{
					m_NormalMapShader->Set("u_Model", model.GetModelMatrix());

					mesh.Bind();

					::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
				}
			}

			m_NormalMapFramebuffer->Unbind();

			// depth map
			m_DepthMapFramebuffer->Bind();
			m_DepthMapShader->Use();

			for (const Model& model : scene.Models)
			{
				if (!model.Visible)
					continue;

				for (const Mesh& mesh : model.Meshes)
				{
					m_DepthMapShader->Set("u_Model", model.GetModelMatrix());

					mesh.Bind();

					::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
				}
			}

			m_DepthMapFramebuffer->Unbind();
		}
		
		m_ScreenFramebuffer->Bind();

		for (const Model& model : scene.Models)
		{
			if (model.Visible)
				Draw(model);
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
		m_NormalMapFramebuffer = std::make_unique<Framebuffer>(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_DepthMapFramebuffer = std::make_unique<Framebuffer>(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		m_SSAOFramebuffer = std::make_unique<Framebuffer>(width, height, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_SSAOHorizontalBlurFramebuffer = std::make_unique<Framebuffer>(width / 2, height / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_SSAOVerticalBlurFramebuffer = std::make_unique<Framebuffer>(width / 2, height / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_BrightExtractFramebuffer = std::make_unique<Framebuffer>(width, height, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_HorizontalBlurFramebuffer = std::make_unique<Framebuffer>(width / 2, height / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		m_VerticalBlurFramebuffer = std::make_unique<Framebuffer>(width / 2, height / 2, GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);

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

	void Renderer::AddToImGui()
	{

		if (::ImGui::Button("Reload Post Processing Shaders"))
		{
			m_NormalMapShader->Load();
			m_DepthMapShader->Load();
			m_SSAOShader->Load();
			m_BlurShader->Load();
			m_BrightExtractShader->Load();
			m_PostProcessingShader->Load();
		}

		::ImGui::Checkbox("Enable Bloom", &m_EnableBloom);
		::ImGui::Checkbox("Enable SSAO", &m_EnableSSAO);
		::ImGui::DragFloat("Gamma", &m_Gamma, 0.05f);
		::ImGui::DragFloat("Bloom Strength", &m_BloomStrength, 0.05f, 0.0f, 5.0f);
		::ImGui::DragFloat("SSAO Strength", &m_SSAOStrength, 0.05f, 0.0f, 2.0f);

		if (::ImGui::CollapsingHeader("Framebuffers"))
		{
			::ImGui::Indent(16.0f);

			::ImGui::Text("Main");
			m_PostProcessFramebuffer->AddToImGui();
			::ImGui::Text("Shadow Map");
			m_ShadowMapFramebuffer->AddToImGui();
			::ImGui::Text("Normal Map");
			m_NormalMapFramebuffer->AddToImGui();
			::ImGui::Text("Depth Map");
			m_DepthMapFramebuffer->AddToImGui();
			::ImGui::Text("SSAO");
			m_SSAOVerticalBlurFramebuffer->AddToImGui();
			::ImGui::Text("Bright Extract");
			m_BrightExtractFramebuffer->AddToImGui();
			::ImGui::Text("Horizontal Blur");
			m_HorizontalBlurFramebuffer->AddToImGui();
			::ImGui::Text("Horizontal and Vertical Blur");
			m_VerticalBlurFramebuffer->AddToImGui();
			::ImGui::Text("Overlay");
			::ImGui::Image((ImTextureID)(intptr_t)m_OverlayTexture->GetID(), ImVec2{ 256.0f, 144.0f });
		}
	}
}