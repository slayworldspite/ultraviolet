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
}