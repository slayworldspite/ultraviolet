#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>

#include "Framebuffer.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Framebuffer::Framebuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment) :
		GLObject{},
		Textures{}
	{	
		::glGenFramebuffers(1, &m_ID);

		AddTexture(textureSettings, attachment);
	}

	Framebuffer::Framebuffer(const TextureSettings& textureSettings, size_t textureCount) :
		GLObject{},
		Textures{}
	{
		assert(textureCount >= 1 && "textureCount isn't valid!");

		::glGenFramebuffers(1, &m_ID);

		Textures.reserve(textureCount);

		for (size_t i{}; i < textureCount; i++)
		{
			AddTexture(textureSettings, static_cast<FramebufferAttachment>(static_cast<size_t>(FramebufferAttachment::Color) + i));
		}
	}

	Framebuffer::Framebuffer(const std::vector<TextureSettings>& textureSettings) :
		GLObject{},
		Textures{}
	{		
		::glGenFramebuffers(1, &m_ID);

		Textures.reserve(textureSettings.size());

		for (size_t i{}; i < textureSettings.size(); i++)
		{
			AddTexture(textureSettings[i], static_cast<FramebufferAttachment>(static_cast<size_t>(FramebufferAttachment::Color) + i));
		}
	}

	Framebuffer::~Framebuffer()
	{
		for (size_t i{}; i < Textures.size(); i++)
			Textures[i].Delete();

		Delete();
	}

	void Framebuffer::AddTexture(const TextureSettings& textureSettings, FramebufferAttachment attachment)
	{
		Bind();

		size_t index{ Textures.size() };

		Textures.emplace_back(textureSettings);

		Textures[index].Bind();
		::glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), GL_TEXTURE_2D, Textures[index].GetID(), 0);
		Textures[index].Unbind();

		Unbind();
	}
	
	void Framebuffer::Clear()
	{
		Bind();

		::glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT
		);

		Unbind();
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
		for (size_t i{}; i < Textures.size(); i++)
		{
			auto id{ Textures[i].GetID() };

			if (::ImPlot::BeginPlot(("Framebuffer ##" + std::to_string(id)).c_str()))
			{
				auto size{ Textures[i].GetSize()};

				::ImPlot::PlotImage(("Framebuffer Texture ##" + std::to_string(id)).c_str(),
					(ImTextureID)(intptr_t)id,
					ImVec2{ 0, 0 },
					ImVec2{ static_cast<float>(size.first), static_cast<float>(size.second) }
				);

				::ImPlot::EndPlot();
			}
		}
	}
}