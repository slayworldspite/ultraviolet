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

		m_DepthMapFrameBuffer = std::make_unique<Framebuffer>(4096, 4096, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		m_DepthMapShader = std::make_unique<Shader>("assets/shaders/DepthMap.vert", "assets/shaders/DepthMap.frag");
		m_DepthMapUniformBuffer = std::make_unique<UniformBuffer>(nullptr, sizeof(DepthMapData), 4);

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

		m_DepthMapFrameBuffer->Bind();
		::glClear(GL_DEPTH_BUFFER_BIT);
		m_DepthMapFrameBuffer->Unbind();

		m_ScreenFramebuffer->Bind();

		::glEnable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);

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
		int windowWidth{};
		int windowHeight{};

		{
			ZoneScoped;

			m_ScreenFramebuffer->Unbind();

			::glDisable(GL_DEPTH_TEST);
			::glDisable(GL_BLEND);

			::glClear(GL_COLOR_BUFFER_BIT);

			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			

			::SDL_GetWindowSizeInPixels(m_Window, &windowWidth, &windowHeight);

			::glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ScreenFramebuffer->GetID());
			::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostProcessFramebuffer->GetID());
			::glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		{
			ZoneScopedN("Post Processing");

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

			// draw to screen
			m_PostProcessingShader->Use();

			m_PostProcessingShader->Set("u_Framebuffer", 0);
			m_PostProcessingShader->Set("u_Blur", 1);
			m_PostProcessingShader->Set("u_EnableBloom", m_EnableBloom);

			m_PostProcessFramebuffer->Texture.Bind(0);
			m_VerticalBlurFramebuffer->Texture.Bind(1);

			::glViewport(0, 0, windowWidth, windowHeight);
			::glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		{
			ZoneScopedN("ImGui Render");

			::ImGui::Render();
			::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		{
			ZoneScopedN("SwapWindow");
			::SDL_GL_SwapWindow(m_Window);
		}
		{
			ZoneScopedN("Update and Render Windows");
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
		{
			ZoneScopedN("MakeCurrent");
			::SDL_GL_MakeCurrent(m_Window, static_cast<SDL_GLContext>(m_GLContext));
		}
		FrameMark;
	}

	void Renderer::SetClearColor(const glm::vec4& color) const
	{
		::glClearColor(color.r, color.g, color.b, color.a);
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
			m_DepthMapFrameBuffer->Texture.Bind(15);
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
		{
			ZoneScopedN("Pre-Draw");

			const DirectionalLight& light{ scene.DirectionalLight };

			glm::vec3 lightDirection{};

			lightDirection.x = glm::cos(glm::radians(light.Pitch)) * glm::sin(glm::radians(light.Yaw));
			lightDirection.y = glm::sin(glm::radians(light.Pitch));
			lightDirection.z = glm::cos(glm::radians(light.Pitch)) * glm::cos(glm::radians(light.Yaw));

			lightDirection = glm::normalize(lightDirection);

			glm::mat4 lightView{ glm::lookAt(-lightDirection * 10.0f, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}) };

			glm::mat4 lightProjection{ glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f) };

			m_LightSpaceMatrix = lightProjection * lightView;

			m_DepthMapUniformBuffer->SetData(&m_LightSpaceMatrix, sizeof(m_LightSpaceMatrix));
		}

		{
			ZoneScopedN("Shadow Pass");

			m_ScreenFramebuffer->Unbind();
			m_DepthMapFrameBuffer->Bind();

			::glViewport(0, 0, 4096, 4096);
			::glEnable(GL_CULL_FACE);
			::glCullFace(GL_FRONT);

			m_DepthMapShader->Use();

			for (const Model& model : scene.Models)
			{
				if (!model.CastsShadow)
					continue;

				for (const Mesh& mesh : model.Meshes)
				{
					m_DepthMapShader->Set("u_Model", model.GetModelMatrix());

					mesh.Bind();

					::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
				}
			}
		}

		{
			ZoneScopedN("Geometry Pass");

			m_DepthMapFrameBuffer->Unbind();
			m_ScreenFramebuffer->Bind();

			::glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
			::glDisable(GL_CULL_FACE);

			for (const Model& model : scene.Models)
			{
				Draw(model);
			}
		}
	}

	void Renderer::SetViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_ScreenFramebuffer = std::make_unique<Framebuffer>(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0);
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
			m_PostProcessingShader->Load();
			m_BrightExtractShader->Load();
			m_BlurShader->Load();
		}

		::ImGui::Checkbox("Enable Bloom", &m_EnableBloom);

		if (::ImGui::CollapsingHeader("Framebuffers"))
		{
			::ImGui::Indent(16.0f);

			m_PostProcessFramebuffer->AddToImGui();
			m_DepthMapFrameBuffer->AddToImGui();
			m_BrightExtractFramebuffer->AddToImGui();
			m_HorizontalBlurFramebuffer->AddToImGui();
			m_VerticalBlurFramebuffer->AddToImGui();
		}
	}
}