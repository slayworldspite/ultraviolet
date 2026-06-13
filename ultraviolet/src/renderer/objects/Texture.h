#pragma once

#include <filesystem>

#include "GLObject.h"

namespace ultraviolet
{
	class Texture : public GLObject
	{
	public:
		Texture(const std::filesystem::path& path);
		virtual ~Texture() override;

		void Bind(size_t index) const;
		virtual void Bind() const override;
		void Unbind(size_t index) const;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}