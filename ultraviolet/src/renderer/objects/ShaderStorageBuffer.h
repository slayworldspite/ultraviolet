#pragma once

#include "GLObject.h"

namespace ultraviolet
{
	class ShaderStorageBuffer : public GLObject
	{
	public:
		ShaderStorageBuffer(const void* data, size_t size, size_t bindingIndex);
		virtual ~ShaderStorageBuffer() override;

		ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
		ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;
		ShaderStorageBuffer(ShaderStorageBuffer&& other);
		ShaderStorageBuffer& operator=(ShaderStorageBuffer&& other);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		void SetData(const void* data, size_t size) const;
		void SetData(const void* data, size_t size, size_t offset) const;
	};
}