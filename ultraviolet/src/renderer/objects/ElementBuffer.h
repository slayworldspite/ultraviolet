#pragma once

#include <cstdint>
#include <vector>

#include "GLObject.h"

namespace ultraviolet
{
	class ElementBuffer : public GLObject
	{
	public:
		ElementBuffer(const std::vector<uint32_t>& indices);
		ElementBuffer(ElementBuffer&& other) noexcept;
		virtual ~ElementBuffer() override;
		ElementBuffer& operator=(ElementBuffer&& other) noexcept;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}