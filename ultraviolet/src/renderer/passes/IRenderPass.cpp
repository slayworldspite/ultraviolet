#include <glad/glad.h>

#include "IRenderPass.h"

namespace ultraviolet
{
	IRenderPass::IRenderPass(const TextureSettings& textureSettings, const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath) :
		Output{ textureSettings, FramebufferAttachment::Color },
		Shader{ vertexShaderPath, fragmentShaderPath }
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

		m_Quad = std::make_unique<Mesh>("quad", vertices, indices);
	}

	void IRenderPass::Draw() const
	{
		m_Quad->Bind();

		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Quad->GetIndexCount()), GL_UNSIGNED_INT, nullptr);
	}
}