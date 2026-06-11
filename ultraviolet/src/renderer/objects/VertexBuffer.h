#pragma once

#include <vector>

#include "GLObject.h"
#include "../mesh/Vertex.h"

namespace ultraviolet
{
	class VertexBuffer : public GLObject
	{
	public:
		VertexBuffer(const std::vector<Vertex>& vertices);
		VertexBuffer(VertexBuffer&& other) noexcept;
		virtual ~VertexBuffer() override;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}