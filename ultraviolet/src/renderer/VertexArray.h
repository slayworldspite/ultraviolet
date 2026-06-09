#pragma once

#include "VertexBuffer.h"

class VertexArray : public GLObject
{
public:
	VertexArray(const VertexBuffer& vertexBuffer, const std::vector<size_t>& sizes);
	virtual ~VertexArray() = default;

	virtual void Bind() const override;
	virtual void Unbind() const override;
	virtual void Delete() override;
};