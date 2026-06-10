#pragma once

#include "VertexBuffer.h"

namespace ultraviolet
{
	class VertexArray : public GLObject
	{
	public:
		VertexArray(const VertexBuffer& vertexBuffer);
		virtual ~VertexArray() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}