#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>

#include "Framebuffer.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Framebuffer::Framebuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment) :
		GLObject{},
		Buffers{}
	{	
		::glGenFramebuffers(1, &m_ID);

		AddBuffer(textureSettings, attachment);
	}

	Framebuffer::Framebuffer(const TextureSettings& textureSettings, size_t textureCount) :
		GLObject{},
		Buffers{}
	{
		assert(textureCount >= 1 && "textureCount isn't valid!");

		::glGenFramebuffers(1, &m_ID);

		Buffers.reserve(textureCount);

		for (size_t i{}; i < textureCount; i++)
		{
			AddBuffer(textureSettings, static_cast<FramebufferAttachment>(static_cast<size_t>(FramebufferAttachment::Color) + i));
		}
	}

	Framebuffer::Framebuffer(const std::vector<TextureSettings>& textureSettings) :
		GLObject{},
		Buffers{}
	{		
		::glGenFramebuffers(1, &m_ID);

		Buffers.reserve(textureSettings.size());

		for (size_t i{}; i < textureSettings.size(); i++)
		{
			AddBuffer(textureSettings[i], static_cast<FramebufferAttachment>(static_cast<size_t>(FramebufferAttachment::Color) + i));
		}
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept
	{
		m_ID = other.m_ID;
		Buffers = std::move(other.Buffers);

		other.m_ID = 0;
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_ID = other.m_ID;
		Buffers = std::move(other.Buffers);

		other.m_ID = 0;

		return *this;
	}

	Framebuffer::~Framebuffer()
	{
		for (size_t i{}; i < Buffers.size(); i++)
			Buffers[i].Delete();

		Delete();
	}

	void Framebuffer::AddBuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment)
	{
		Bind();

		size_t index{ Buffers.size() };

		Buffers.emplace_back(textureSettings);

		Buffers[index].Bind();
		::glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), GL_TEXTURE_2D, Buffers[index].GetID(), 0);
		Buffers[index].Unbind();

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
		for (size_t i{}; i < Buffers.size(); i++)
		{
			auto id{ Buffers[i].GetID() };

			if (::ImPlot::BeginPlot(("Framebuffer ##" + std::to_string(id)).c_str()))
			{
				auto size{ Buffers[i].GetSize()};

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