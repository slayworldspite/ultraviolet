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
		virtual ~VertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}