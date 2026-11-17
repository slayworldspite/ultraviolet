#include <format>
#include <random>
#include <array>

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
		m_RenderQueue{},
		m_WireframeMode{ false },
		m_Gamma{ 2.2f },
		m_EnableFXAA{ true },
		m_ViewportWidth{},
		m_ViewportHeight{}
	{
		Logger::Log("initializing renderer...", Logger::Level::Info);

		CreateContext(window);
		Logger::Log(std::format("opengl v{}", reinterpret_cast<const char*>(::glGetString(GL_VERSION))), Logger::Level::Info);

		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		::SDL_GL_SetSwapInterval(1);

		InitializeImGui();

		MaterialShaderManager::Initialize();

		auto windowSize{ window.GetWindowSize() };
		SetViewport(windowSize.first, windowSize.second);

		CreateGBuffer();
		CreateScreenQuad();
		
		InitializeRenderPasses();
		InitializePostProcessingShader();

		Logger::Log("renderer initialized!", Logger::Level::Info);
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

		m_GBuffer->Clear();

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

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		::SDL_GL_SwapWindow(m_Window);

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();

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

	void Renderer::Execute()
	{
		m_RenderQueue.Sort();
		const std::vector<DrawItem>& drawItems{ m_RenderQueue.Get() };

		m_GBuffer->Bind();

		for (const DrawItem& drawItem : drawItems)
		{
			Draw(drawItem);
		}

		m_GBuffer->Unbind();

		m_RenderQueue.Clear();

		PresentFrame();
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
	
		CreateGBuffer();
		CreateScreenQuad();
		InitializeRenderPasses();
	}

	void Renderer::AddToImGui()
	{
		::ImGui::DragFloat("Gamma", &m_Gamma, 0.05f);
		::ImGui::Checkbox("Wireframe Mode", &m_WireframeMode);
		::ImGui::Checkbox("FXAA", &m_EnableFXAA);
		
		if (::ImGui::Button("Reload Material Shaders"))
			MaterialShaderManager::Reload();

		if (::ImGui::Button("Reload Deferred Shader"))
			m_DeferredPass->Shader.Load();
		
		if (::ImGui::Button("Reload Post Processing Shader"))
			m_PostProcessingShader->Load();

		if (::ImGui::CollapsingHeader("G Buffer"))
			m_GBuffer->AddToImGui();
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

	void Renderer::CreateGBuffer()
	{
		TextureSettings textureSettings{
			.Width = m_ViewportWidth,
			.Height = m_ViewportHeight,
			.InternalFormat = TextureInternalFormat::Rgba16f,
			.Format = TextureFormat::Rgba,
			.MinFiltering = TextureFilter::Linear,
			.MagFiltering = TextureFilter::Linear,
			.Wrap = TextureWrap::ClampToBorder,
			.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f }
		};

		m_GBuffer = std::make_unique<Framebuffer>(textureSettings, 4);

		TextureSettings depthTextureSettings = textureSettings;
		depthTextureSettings.InternalFormat = TextureInternalFormat::DepthComponent;
		depthTextureSettings.Format = TextureFormat::DepthComponent;

		m_GBuffer->AddBuffer(depthTextureSettings, FramebufferAttachment::Depth);

		std::array<GLenum, 4> attachments{};

		for (size_t i{}; i < attachments.size(); i++)
		{
			attachments[i] = static_cast<GLenum>(static_cast<size_t>(GL_COLOR_ATTACHMENT0) + i);
		}

		m_GBuffer->Bind();
		::glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
	}

	void Renderer::CreateScreenQuad()
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(4);

		vertices.push_back(Vertex{
			.Position{ -1, -1, 0 },
			.TextureCoordinates{ 0, 0 }
		});

		vertices.push_back(Vertex{
			.Position{ 1, -1, 0 },
			.TextureCoordinates { 1, 0 }
		});

		vertices.push_back(Vertex{
			.Position{ -1, 1, 0 },
			.TextureCoordinates { 0, 1 }
		});

		vertices.push_back(Vertex{
			.Position{ 1, 1, 0 },
			.TextureCoordinates { 1, 1 }
		});

		std::vector<uint32_t> indices{ 0, 1, 2, 1, 3, 2 };

		m_ScreenQuad = std::make_unique<Mesh>("screen_quad", vertices, indices);
	}

	void Renderer::InitializeRenderPasses()
	{
		m_DeferredPass = std::make_unique<DeferredPass>(m_ViewportWidth, m_ViewportHeight);
	}

	void Renderer::InitializePostProcessingShader()
	{
		m_PostProcessingShader = std::make_unique<Shader>("assets/shaders/passes/Pass.vert", "assets/shaders/passes/PostProcessing.frag");
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
		if (std::shared_ptr<Material> material{ drawItem.Mesh->Material })
		{
			material->Apply();
			MaterialShaderManager::Get(material->Type)->Set("u_Model", drawItem.Transform);
		}

		if (drawItem.Mesh)
		{
			drawItem.Mesh->Bind();

			::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(drawItem.Mesh->GetIndexCount()), GL_UNSIGNED_INT, nullptr);
		}
	}

	void Renderer::PresentFrame() const
	{
		if (m_WireframeMode)
			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		RenderPassInfo deferredPassInfo{};

		for (size_t i{}; i < m_GBuffer->Buffers.size(); i++)
		{
			if (i == m_GBuffer->Buffers.size() - 1)
			{
				deferredPassInfo.DepthAttachment = RenderTarget{
					.Texture{ &m_GBuffer->Buffers[i] },
					.Attachment{ i }
				};

				continue;
			}

			RenderTarget target{
				.Texture{ &m_GBuffer->Buffers[i] },
				.Attachment{ i }
			};

			deferredPassInfo.ColorAttachments.push_back(target);
		}

		m_DeferredPass->Execute(deferredPassInfo);

		BindBackbuffer();

		m_PostProcessingShader->Use();
		m_PostProcessingShader->Set("u_Framebuffer", 0);
		m_PostProcessingShader->Set("u_Gamma", m_Gamma);
		m_PostProcessingShader->Set("u_Resolution", glm::vec2{ m_ViewportWidth, m_ViewportHeight });
		m_PostProcessingShader->Set("u_EnableFXAA", m_EnableFXAA);

		m_DeferredPass->Output.Buffers[0].Bind();
		m_ScreenQuad->Bind();

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_ScreenQuad->GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::BindBackbuffer() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}