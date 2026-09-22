#include <glad/glad.h>
#include <imgui/imgui.h>

#include "Framebuffer.h"

namespace ultraviolet
{
	Framebuffer::Framebuffer(int width, int height, int internalFormat, int format, int type, int attachment, bool multisampled) :
		GLObject{},
		Texture{ width, height, internalFormat, format, type, multisampled }
	{
		::glGenFramebuffers(1, &m_ID);
		Bind();

		Texture.Bind();

		if (multisampled)
			::glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, Texture.GetID(), 0);
		else
			::glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, Texture.GetID(), 0);

		Texture.Unbind();
		Unbind();
	}

	Framebuffer::~Framebuffer()
	{
		Texture.Delete();
		Delete();
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

	void Framebuffer::AddToImGui()
	{
		auto size{ Texture.GetSize() };
		::ImGui::Image((ImTextureID)(intptr_t)Texture.GetID(), ImVec2{ static_cast<float>(size.first), static_cast<float>(size.second) });
	}
}