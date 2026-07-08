#include <glad/glad.h>

#include "Renderbuffer.h"

namespace ultraviolet
{
	Renderbuffer::Renderbuffer(const Framebuffer& framebuffer, bool multisampled) :
		GLObject{}
	{

		::glGenRenderbuffers(1, &m_ID);
		Bind();

		framebuffer.Bind();

		auto size{ framebuffer.Texture.GetSize() };

		if (multisampled)
			::glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, size.first, size.second);
		else
			::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.first, size.second);
		::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_ID);

		Unbind();
	}

	Renderbuffer::~Renderbuffer()
	{
		Delete();
	}
	
	void Renderbuffer::Bind() const
	{
		::glBindRenderbuffer(GL_RENDERBUFFER, m_ID);
	}

	void Renderbuffer::Unbind() const
	{
		::glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void Renderbuffer::Delete()
	{
		::glDeleteRenderbuffers(1, &m_ID);
	}
}