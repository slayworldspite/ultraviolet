#pragma once

#include <memory>

#include "../objects/VertexArray.h"
#include "../objects/ElementBuffer.h"
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
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<IMaterial> material);
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;

		void Bind() const;
		size_t GetIndexCount() const;
		void AddToImGui(const std::string& id, size_t index);

	private:
		VertexBuffer m_VertexBuffer;
		ElementBuffer m_ElementBuffer;
		VertexArray m_VertexArray;
		size_t m_IndexCount;
	};
}