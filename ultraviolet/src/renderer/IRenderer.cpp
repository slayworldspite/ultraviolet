#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "IRenderer.h"

namespace ultraviolet
{
	IRenderer::IRenderer(const NativeWindow& window) :
		m_Window{ window.GetNativeHandle() }
	{

	}

	void IRenderer::BindBackbuffer() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void IRenderer::DrawElements(size_t indexCount) const
	{
		::glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
	}

	void IRenderer::SwapWindow() const
	{
		::SDL_GL_SwapWindow(m_Window);
	}

	::SDL_Window* IRenderer::GetNativeHandle()
	{
		return m_Window;
	}

	Mesh IRenderer::CreateScreenQuad()
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

		return Mesh{ "screen_quad", vertices, indices };
	}
}