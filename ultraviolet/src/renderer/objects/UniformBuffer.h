#pragma once

#include "GLObject.h"

namespace ultraviolet
{
	class UniformBuffer : public GLObject
 	{
	public:
		UniformBuffer(const void* data, size_t size, size_t bindingPoint);
		virtual ~UniformBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
		
		void SetData(const void* data, size_t size) const;
	};
}