#include <imgui/imgui.h>

#include "Mesh.h"

namespace ultraviolet
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<IMaterial> material) :
		Material{ std::move(material) },
		m_VertexBuffer{ vertices },
		m_ElementBuffer{ indices },
		m_VertexArray{ m_VertexBuffer, m_ElementBuffer },
		m_IndexCount{ indices.size() }
	{

	}

	void Mesh::Bind() const
	{
		m_VertexArray.Bind();
	}

	size_t Mesh::GetIndexCount() const
	{
		return m_IndexCount;
	}

	void Mesh::AddToImGui(const std::string& id, size_t index)
	{
		if (::ImGui::CollapsingHeader(("Mesh #" + std::to_string(index) + "##" + id).c_str()))
		{
			::ImGui::Text("Index Count: %d", m_IndexCount);
			Material->AddToImGui(id, index);
		}
	}
}