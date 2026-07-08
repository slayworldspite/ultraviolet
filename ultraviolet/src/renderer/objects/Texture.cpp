#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Texture.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Texture::Texture(const std::filesystem::path& path, TextureType type) :
		GLObject{},
		Type{ type },
		m_Multisampled{ false }
	{
			Logger::Log("loading " + path.string(), Logger::Level::Info);

		int width{};
		int height{};
		int channelCount{};

		::stbi_set_flip_vertically_on_load(1);
		uint8_t* data{ ::stbi_load(path.string().c_str(), &width, &height, &channelCount, 0) };
		
		m_Size.first = width;
		m_Size.second = height;

		::glGenTextures(1, &m_ID);
		Bind();

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (data)
		{
			GLenum internalFormat{};
			GLenum imageFormat{};

			switch (channelCount)
			{
				case 1:
					internalFormat = GL_RED;
					imageFormat = GL_RED;
					break;

				case 3:
					if (type == TextureType::Diffuse || type == TextureType::Overlay)
					{
						internalFormat = GL_SRGB;
						imageFormat = GL_RGB;
					}
					else
					{
						internalFormat = GL_RGB;
						imageFormat = GL_RGB;
					}
					break;

				case 4:
					if (type == TextureType::Diffuse || type == TextureType::Overlay)
					{
						internalFormat = GL_SRGB_ALPHA;
						imageFormat = GL_RGBA;
					}
					else
					{
						internalFormat = GL_RGBA;
						imageFormat = GL_RGBA;
					}
					break;
			}

			::glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
			::glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			Logger::Log("failed to load texture!", Logger::Level::Error);
		}

		::stbi_image_free(data);

		Unbind();
	}

	Texture::Texture(int width, int height, int internalFormat, int format, int type, bool multisampled) :
		GLObject{},
		m_Multisampled{ multisampled }
	{
		m_Size.first = width;
		m_Size.second = height;

		::glGenTextures(1, &m_ID);
		Bind();

		int target{ multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D };

		if (m_Multisampled)
			::glTexImage2DMultisample(target, 4, internalFormat, width, height, GL_TRUE);
		else
			::glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, nullptr);

		::glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		::glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		::glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		::glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Texture::~Texture()
	{
		Delete();
	}

	std::pair<int, int> Texture::GetSize() const
	{
		return m_Size;
	}

	void Texture::Bind(size_t index) const
	{
		::glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + static_cast<int>(index)));

		if (m_Multisampled)
			::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID);
		else
			::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Bind() const
	{
		::glActiveTexture(GL_TEXTURE0);

		if (m_Multisampled)
			::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ID);
		else
			::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Unbind(size_t index) const
	{
		::glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));

		if (m_Multisampled)
			::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		else
			::glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Unbind() const
	{
		::glActiveTexture(GL_TEXTURE0);

		if (m_Multisampled)
			::glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		else
			::glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Delete()
	{
		::glDeleteTextures(1, &m_ID);
	}
}