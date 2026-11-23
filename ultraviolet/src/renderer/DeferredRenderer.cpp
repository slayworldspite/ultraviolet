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

#include "DeferredRenderer.h"
#include "materials/MaterialShaderManager.h"
#include "../utils/Logger.h"
#include "RenderContext.h"

#define AMBIENT_LIGHT_BINDING 0
#define DIRECTIONAL_LIGHT_BINDING 1
#define POINT_LIGHT_BINDING 0
#define CAMERA_DATA_BINDING 2

namespace
{
	struct DepthMapData
	{
		glm::mat4 LightSpaceMatrix;
	};

	struct AmbientLightData
	{
		glm::vec3 Color;
		float Intensity;
	};

	struct DirectionalLightData
	{
		glm::vec4 Color;
		glm::vec4 Direction;
		float Intensity;
		int Enabled;
		uint8_t _pad[8];
	};

	struct PointLightData
	{
		glm::vec4 Color;
		glm::vec4 Position;
		float Intensity;
		float Radius;   
		int Enabled;
	};

	struct CameraData
	{
		glm::mat4 Projection;
		glm::mat4 View;
		glm::vec3 Position;
	};
}

namespace ultraviolet
{
	DeferredRenderer::DeferredRenderer(const NativeWindow& window) :
		IRenderer{ window },
		m_GBufferSettings{
			.Width = window.GetWindowSize().first,
			.Height = window.GetWindowSize().second,
			.InternalFormat = TextureInternalFormat::Rgba16f,
			.Format = TextureFormat::Rgba,
			.MinFiltering = TextureFilter::Linear,
			.MagFiltering = TextureFilter::Linear,
			.Wrap = TextureWrap::ClampToBorder,
			.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f }
		},
		m_GBuffer{ m_GBufferSettings, 4 },
		m_ScreenQuad{ Mesh::CreateScreenQuad() },
		m_DeferredPass{ m_GBufferSettings.Width, m_GBufferSettings.Height },
		m_PostProcessingShader{ 
			"assets/shaders/passes/Pass.vert", 
			"assets/shaders/passes/PostProcessing.frag" 
		},
		m_AmbientLightBuffer{ nullptr, sizeof(AmbientLightData), AMBIENT_LIGHT_BINDING },
		m_DirectionalLightBuffer{ nullptr, sizeof(DirectionalLightData), DIRECTIONAL_LIGHT_BINDING },
		m_PointLightBuffer{ nullptr, sizeof(PointLightData), POINT_LIGHT_BINDING },
		m_CameraBuffer{ nullptr, sizeof(CameraData), CAMERA_DATA_BINDING },
		m_LastPointLightCount{ 0 },
		m_RenderQueue{},
		m_WireframeMode{ false },
		m_Gamma{ 2.2f },
		m_EnableFXAA{ true },
		m_ViewportWidth{ m_GBufferSettings.Width },
		m_ViewportHeight{ m_GBufferSettings.Height }
	{
		Logger::Log("initializing renderer...", Logger::Level::Info);

		assert(RenderContext::IsInitialized() && "render context isn't initialized!");

		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		::SDL_GL_SetSwapInterval(1);

		InitializeImGui();

		MaterialShaderManager::Initialize();

		AddDepthBuffer();
		AttachGBuffer();

		Logger::Log("renderer initialized!", Logger::Level::Info);
	}

	DeferredRenderer::~DeferredRenderer()
	{
		::ImGui_ImplOpenGL3_Shutdown();
		::ImGui_ImplSDL3_Shutdown();
		::ImPlot::DestroyContext();
		::ImGui::DestroyContext();
	}

	void DeferredRenderer::BeginFrame()
	{
		ZoneScoped;

		::glEnable(GL_DEPTH_TEST);
		::glEnable(GL_BLEND);
		::glEnable(GL_CULL_FACE);

		m_GBuffer.Clear();

		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);

		::glPolygonMode(GL_FRONT_AND_BACK, m_WireframeMode ? GL_LINE : GL_FILL);

		::ImGui_ImplOpenGL3_NewFrame();
		::ImGui_ImplSDL3_NewFrame();
		::ImGui::NewFrame();
	}

	void DeferredRenderer::EndFrame()
	{
		ZoneScoped;

		::ImGui::Render();
		::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SwapWindow();

		::ImGui::UpdatePlatformWindows();
		::ImGui::RenderPlatformWindowsDefault();

		FrameMark;
	}

	void DeferredRenderer::Submit(const Mesh& mesh, const glm::mat4& transform)
	{
		DrawItem drawItem{
			.Mesh = &mesh,
			.Transform = transform
		};

		drawItem.GenerateSortKey();

		m_RenderQueue.Submit(drawItem);
	}

	void DeferredRenderer::Submit(const Model& model)
	{
		const glm::mat4& transform{ model.GetModelMatrix() };

		for (const Mesh& mesh : model.Meshes)
			Submit(mesh, transform);
	}

	void DeferredRenderer::Submit(const Entity& entity)
	{
		Submit(entity.RenderObject);
	}

	void DeferredRenderer::Submit(const Scene& scene)
	{
		for (const Model& model : scene.Models)
			Submit(model);

		for (const Entity& entity : scene.Entities)
			Submit(entity);

		Submit(scene.GetLightingData());
		Submit(scene.Camera);
	}

	void DeferredRenderer::Submit(const LightingData& data)
	{
		// ambient
		AmbientLightData ambientLightData{
			.Color = data.AmbientLight.Color,
			.Intensity = data.AmbientLight.Intensity
		};

		m_AmbientLightBuffer.SetData(&ambientLightData, sizeof(AmbientLightData));

		// directional
		glm::vec3 direction{};

		float yaw{ glm::radians(data.DirectionalLight.Yaw) };
		float pitch{ glm::radians(data.DirectionalLight.Pitch) };

		direction.x = cos(yaw) * cos(pitch);
		direction.y = sin(pitch);
		direction.z = sin(yaw) * cos(pitch);

		DirectionalLightData directionalLightData{
			.Color{ glm::vec4{ data.DirectionalLight.Color, 0.0f } },
			.Direction{ glm::vec4{ direction, 0.0f } },
			.Intensity{ data.DirectionalLight.Intensity },
			.Enabled{ data.DirectionalLight.Enabled }
		};

		m_DirectionalLightBuffer.SetData(&directionalLightData, sizeof(DirectionalLightData));

		// point
		if (data.PointLights.size() != m_LastPointLightCount)
		{
			m_PointLightBuffer = ShaderStorageBuffer{ nullptr, data.PointLights.size() * sizeof(PointLightData), POINT_LIGHT_BINDING};

			m_LastPointLightCount = data.PointLights.size();
		}

		std::vector<PointLightData> pointLightDatas{};
		pointLightDatas.reserve(data.PointLights.size());

		for (const PointLight& pointLight : data.PointLights)
		{
			PointLightData pointLightData{
				.Color = glm::vec4{ pointLight.Color, 0.0f },
				.Position = glm::vec4{ pointLight.Position, 0.0f },
				.Intensity = pointLight.Intensity,
				.Radius = pointLight.Intensity / 0.001f,
				.Enabled = pointLight.Enabled
			};

			pointLightDatas.push_back(pointLightData);
		}

		m_PointLightBuffer.SetData(pointLightDatas.data(), pointLightDatas.size() * sizeof(PointLightData));
	}

	void DeferredRenderer::Submit(const ProjectionCamera& camera)
	{
		glm::mat4 roll{ glm::rotate(glm::mat4{ 1.0f }, glm::radians(-camera.Roll), camera.GetFront()) };

		CameraData data{
			.Projection{
				glm::perspective(
					glm::radians(camera.FOV),
					static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight),
					0.1f,
					100.0f
				)
			},
			.View{
				glm::lookAt(
					camera.Position,
					camera.Position + camera.GetFront(),
					camera.GetWorldUp() * glm::mat3{ roll })
			},
			.Position{ camera.Position }
		};

		m_CameraBuffer.SetData(&data, sizeof(CameraData));
	}

	void DeferredRenderer::Execute()
	{
		ZoneScoped;

		m_RenderQueue.Sort();
		const std::vector<DrawItem>& drawItems{ m_RenderQueue.Get() };

		m_GBuffer.Bind();

		for (const DrawItem& drawItem : drawItems)
		{
			Draw(drawItem);
		}

		m_GBuffer.Unbind();

		m_RenderQueue.Clear();

		PresentFrame();
	}

	void DeferredRenderer::SetViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);

		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	void DeferredRenderer::SetWireframeMode(bool value)
	{
		m_WireframeMode = value;
	}

	void DeferredRenderer::SetVSync(bool value) const
	{
		::SDL_GL_SetSwapInterval(value);
	}

	void DeferredRenderer::Resize(int width, int height)
	{
		SetViewport(width, height);
	
		m_GBufferSettings.Width = m_ViewportWidth;
		m_GBufferSettings.Height = m_ViewportHeight;

		m_GBuffer = Framebuffer{ m_GBufferSettings, 4 };

		AddDepthBuffer();
		AttachGBuffer();
		m_ScreenQuad = Mesh::CreateScreenQuad();

		m_DeferredPass = DeferredPass{ m_ViewportWidth, m_ViewportHeight };
	}

	void DeferredRenderer::AddToImGui()
	{
		::ImGui::DragFloat("Gamma", &m_Gamma, 0.05f);
		::ImGui::Checkbox("Wireframe Mode", &m_WireframeMode);
		::ImGui::Checkbox("FXAA", &m_EnableFXAA);
		
		if (::ImGui::Button("Reload Material Shaders"))
			MaterialShaderManager::Reload();

		if (::ImGui::Button("Reload Deferred Shader"))
			m_DeferredPass.Shader.Load();
		
		if (::ImGui::Button("Reload Post Processing Shader"))
			m_PostProcessingShader.Load();

		if (::ImGui::CollapsingHeader("G Buffer"))
			m_GBuffer.AddToImGui();
	}

	void DeferredRenderer::AddDepthBuffer()
	{
		TextureSettings depthTextureSettings = m_GBufferSettings;
		depthTextureSettings.InternalFormat = TextureInternalFormat::DepthComponent;
		depthTextureSettings.Format = TextureFormat::DepthComponent;

		m_GBuffer.AddBuffer(depthTextureSettings, FramebufferAttachment::Depth);
	}

	void DeferredRenderer::AttachGBuffer()
	{
		std::array<GLenum, 4> attachments{};

		for (size_t i{}; i < attachments.size(); i++)
		{
			attachments[i] = static_cast<GLenum>(static_cast<size_t>(GL_COLOR_ATTACHMENT0) + i);
		}

		m_GBuffer.Bind();
		::glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
	}

	void DeferredRenderer::InitializeImGui()
	{
		::IMGUI_CHECKVERSION();
		::ImGui::CreateContext();
		
		::ImPlot::CreateContext();

		::ImGuiIO& io{ ::ImGui::GetIO() };
		io.ConfigFlags |=
			::ImGuiConfigFlags_DockingEnable |
			::ImGuiConfigFlags_ViewportsEnable |
			::ImGuiConfigFlags_NavEnableKeyboard;

		::ImGui_ImplSDL3_InitForOpenGL(GetNativeHandle(), RenderContext::GetContext());
		::ImGui_ImplOpenGL3_Init();
	}

	void DeferredRenderer::Draw(const DrawItem& drawItem) const
	{
		ZoneScoped;

		if (std::shared_ptr<Material> material{ drawItem.Mesh->Material })
		{
			material->Apply();
			MaterialShaderManager::Get(material->Type)->Set("u_Model", drawItem.Transform);
		}

		if (drawItem.Mesh)
		{
			drawItem.Mesh->Bind();

			DrawElements(drawItem.Mesh->GetIndexCount());
		}
	}

	void DeferredRenderer::PresentFrame()
	{
		ZoneScoped;

		if (m_WireframeMode)
			::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		RenderPassInfo deferredPassInfo{};

		for (size_t i{}; i < m_GBuffer.Buffers.size(); i++)
		{
			if (i == m_GBuffer.Buffers.size() - 1)
			{
				deferredPassInfo.DepthAttachment = RenderTarget{
					.Texture{ &m_GBuffer.Buffers[i] },
					.Attachment{ i }
				};

				continue;
			}

			RenderTarget target{
				.Texture{ &m_GBuffer.Buffers[i] },
				.Attachment{ i }
			};

			deferredPassInfo.ColorAttachments.push_back(target);
		}

		m_DeferredPass.Shader.Set("u_PointLightCount", static_cast<int>(m_LastPointLightCount));

		m_DeferredPass.Execute(deferredPassInfo);

		BindBackbuffer();

		m_PostProcessingShader.Use();
		m_PostProcessingShader.Set("u_Framebuffer", 0);
		m_PostProcessingShader.Set("u_Gamma", m_Gamma);
		m_PostProcessingShader.Set("u_Resolution", glm::vec2{ m_ViewportWidth, m_ViewportHeight });
		m_PostProcessingShader.Set("u_EnableFXAA", m_EnableFXAA);

		m_DeferredPass.Output.Buffers[0].Bind();
		m_ScreenQuad.Bind();

		DrawElements(m_ScreenQuad.GetIndexCount());
	}
}