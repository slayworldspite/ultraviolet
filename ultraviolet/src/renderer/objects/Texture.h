#pragma once

#include <filesystem>

#include "GLObject.h"
#include "TextureType.h"

namespace ultraviolet
{
	class Texture : public GLObject
	{
	public:
		TextureType Type;

		Texture(const std::filesystem::path& path, TextureType type);
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