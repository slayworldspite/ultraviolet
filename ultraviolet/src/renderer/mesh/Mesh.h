#pragma once

#include <memory>

#include "../objects/VertexArray.h"
#include "../objects/ElementBuffer.h"
#include "Vertex.h"
#include "../materials/Material.h"

namespace ultraviolet
{
	class Mesh
	{
	public:
		std::string ID;
		std::shared_ptr<Material> Material;
		std::vector<Vertex> Vertices;

	public:
		Mesh(std::string_view id, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;

		void Bind() const;
		size_t GetIndexCount() const;
		void AddToImGui(const std::string& id);

	private:
		VertexBuffer m_VertexBuffer;
		ElementBuffer m_ElementBuffer;
		VertexArray m_VertexArray;
		size_t m_IndexCount;
	};
}