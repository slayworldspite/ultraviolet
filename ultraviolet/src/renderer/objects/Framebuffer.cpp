#include <glad/glad.h>

#include "Framebuffer.h"

namespace ultraviolet
{
	Framebuffer::Framebuffer(int width, int height, uint32_t format, uint32_t attachment) :
		m_Width{ width },
		m_Height{ height }
	{
		::glGenFramebuffers(1, &m_ID);
		Bind();

		::glGenTextures(1, &m_Texture);
		::glBindTexture(GL_TEXTURE_2D, m_Texture);

		::glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, nullptr);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		Bind();

		::glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_Texture, 0);
		::glDrawBuffer(GL_NONE);
		::glReadBuffer(GL_NONE);

		Unbind();
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept
	{
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		m_Texture = other.m_Texture;
		m_ID = other.m_ID;
		other.m_ID = 0;
	}

	Framebuffer::~Framebuffer()
	{
		Delete();
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteFramebuffers(1, &m_ID);
			glDeleteTextures(1, &m_Texture);

			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_Texture = other.m_Texture;
			m_ID = other.m_ID;
			other.m_ID = 0;
		}

		return *this;
	}

	void Framebuffer::Bind() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	void Framebuffer::Unbind() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Delete()
	{
		::glDeleteFramebuffers(1, &m_ID);
	}

	std::pair<int, int> Framebuffer::GetSize() const
	{
		return { m_Width, m_Height };
	}

	uint32_t Framebuffer::GetTexture() const
	{
		return m_Texture;
	}
}