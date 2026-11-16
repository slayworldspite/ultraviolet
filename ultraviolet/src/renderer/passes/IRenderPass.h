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
		virtual ~IRenderPass() = default;

		virtual void Execute(const RenderPassInfo& info) const = 0;

	protected:
		void Draw() const;

	private:
		std::unique_ptr<Mesh> m_Quad;
	};
}