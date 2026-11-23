#pragma once

#include "../Shader.h"
#include "../objects/Framebuffer.h"
#include "RenderPassInfo.h"
#include "../mesh/Mesh.h"

namespace ultraviolet
{
	class IRenderPass
	{
	public:
		Framebuffer Output;
		Shader Shader;

	public:
		IRenderPass(const TextureSettings& textureSettings, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
		
		IRenderPass(const IRenderPass&) = delete;
		IRenderPass& operator=(const IRenderPass&) = delete;
		IRenderPass(IRenderPass&& other) noexcept;
		IRenderPass& operator=(IRenderPass&& other) noexcept;
		
		virtual ~IRenderPass() = default;

		virtual void Execute(const RenderPassInfo& info) const = 0;

	protected:
		void Draw() const;

	private:
		Mesh m_ScreenQuad;
	};
}