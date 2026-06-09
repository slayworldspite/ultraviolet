#pragma once

#include <vector>

#include "GLObject.h"

class VertexBuffer : public GLObject
{
public:
	VertexBuffer(const std::vector<float>& data);
	virtual ~VertexBuffer() = default;

	virtual void Bind() const override;
	virtual void Unbind() const override;
	virtual void Delete() override;
};