#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>

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

	void Framebuffer::AddToImGui(const std::string& id)
	{
		if (::ImPlot::BeginPlot(("Framebuffer ##" + id).c_str()))
		{
			auto size{ Texture.GetSize() };

			::ImPlot::PlotImage(("Framebuffer Texture ##" + id).c_str(),
				(ImTextureID)(intptr_t)Texture.GetID(),
				ImVec2{ 0, 0 }, 
				ImVec2{ static_cast<float>(size.first), static_cast<float>(size.second) }
			);

			::ImPlot::EndPlot();
		}
	}
}