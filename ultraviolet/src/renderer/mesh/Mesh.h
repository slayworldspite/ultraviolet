#pragma once

#include <memory>

#include "../objects/VertexArray.h"
#include "Vertex.h"
#include "IMaterial.h"
#include "TextureMaterial.h"
#include "ColorMaterial.h"

namespace ultraviolet
{
	class Mesh
	{
	public:
		std::shared_ptr<IMaterial> Material;

	public:
		Mesh(const std::vector<Vertex>& vertices, std::shared_ptr<IMaterial> material);
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;

		void Bind() const;
		size_t GetVertexCount() const;

	private:
		VertexBuffer m_VertexBuffer;
		VertexArray m_VertexArray;
		size_t m_VertexCount;
	};
}