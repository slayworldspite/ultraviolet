#include <glad/glad.h>

#include "IRenderPass.h"

namespace ultraviolet
{
	IRenderPass::IRenderPass(const TextureSettings& textureSettings, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) :
		Output{ textureSettings, FramebufferAttachment::Color },
		Shader{ vertexShaderPath, fragmentShaderPath },
		m_ScreenQuad{ Mesh::CreateScreenQuad() }
	{

	}

	IRenderPass::IRenderPass(IRenderPass&& other) noexcept :
		Output{ std::move(other.Output) },
		Shader{ std::move(other.Shader) },
		m_ScreenQuad{ std::move(other.m_ScreenQuad) }
	{

	}

	IRenderPass& IRenderPass::operator=(IRenderPass&& other) noexcept
	{
		if (this == &other)
			return *this;

		Output = std::move(other.Output);
		Shader = std::move(other.Shader);
		m_ScreenQuad = std::move(other.m_ScreenQuad);

		return *this;
	}

	void IRenderPass::Draw() const
	{
		m_ScreenQuad.Bind();

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_ScreenQuad.GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}
}