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

		void Bind(int index) const;
		virtual void Bind() const override;
		void Unbind(int index) const;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}