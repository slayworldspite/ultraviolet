#include <imgui/imgui.h>
#include <array>
#include <numeric>

#include "Mesh.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Mesh::Mesh(std::string_view id, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) :
		ID{ id },
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

	void Mesh::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader((ID + "##" + id).c_str()))
		{
			::ImGui::Text("Index Count: %d", m_IndexCount);

			if (Material)
				Material->AddToImGui(ID);
		}
	}

	Mesh Mesh::CreateScreenQuad()
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(4);

		vertices.push_back(Vertex{
			.Position{ -1, -1, 0 },
			.TextureCoordinates{ 0, 0 }
			});

		vertices.push_back(Vertex{
			.Position{ 1, -1, 0 },
			.TextureCoordinates { 1, 0 }
			});

		vertices.push_back(Vertex{
			.Position{ -1, 1, 0 },
			.TextureCoordinates { 0, 1 }
			});

		vertices.push_back(Vertex{
			.Position{ 1, 1, 0 },
			.TextureCoordinates { 1, 1 }
			});

		std::vector<uint32_t> indices{ 0, 1, 2, 1, 3, 2 };

		return Mesh{ "screen_quad", vertices, indices };
	}
}