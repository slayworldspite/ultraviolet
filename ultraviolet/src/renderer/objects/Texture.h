#pragma once

#include <filesystem>
#include <string_view>

#include "GLObject.h"
#include "TextureType.h"
#include "TextureSettings.h"

namespace ultraviolet
{
	class Texture : public GLObject
	{
	public:
		TextureType Type;

		Texture(const std::filesystem::path& path, TextureType type);
		Texture(const TextureSettings& settings);

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;

		virtual ~Texture() override;

		std::pair<int, int> GetSize() const;

		void Bind(size_t index) const;
		virtual void Bind() const override;
		void Unbind(size_t index) const;
		virtual void Unbind() const override;
		virtual void Delete() override;

	private:
		std::pair<int, int> m_Size;
	};
}