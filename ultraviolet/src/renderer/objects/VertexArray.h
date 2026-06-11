#pragma once

#include "VertexBuffer.h"

namespace ultraviolet
{
	class VertexArray : public GLObject
	{
	public:
		VertexArray(const VertexBuffer& vertexBuffer);
		VertexArray(VertexArray&& other) noexcept;
		virtual ~VertexArray() override;
		VertexArray& operator=(VertexArray&& other) noexcept;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}