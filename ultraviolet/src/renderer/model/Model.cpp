#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "Model.h"

namespace ultraviolet
{
	Model::Model(std::string_view id, std::vector<Mesh>& meshes) :
		ID{ id },
		Meshes{ std::move(meshes) },
		Position{},
		Scale{ 1.0 },
		Rotation{},
		CastsShadow{ true },
		Visible{ true }
	{
		for (const Mesh& mesh : Meshes)
		{
			for (const Vertex& vertex : mesh.Vertices)
			{
				BoundingBox.Min = glm::min(BoundingBox.Min, vertex.Position);
				BoundingBox.Max = glm::max(BoundingBox.Max, vertex.Position);
			}
		}
	}

	glm::mat4 Model::GetModelMatrix() const
	{
		glm::mat4 transform{ 1.0f };

		transform = glm::translate(transform, Position);
		transform = glm::rotate(transform, glm::radians(Rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		transform = glm::rotate(transform, glm::radians(Rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
		transform = glm::rotate(transform, glm::radians(Rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, Scale);

		return transform;
	}

	void Model::AddToImGui()
	{
		if (::ImGui::CollapsingHeader(ID.c_str()))
		{
			::ImGui::Checkbox(("Visible ##" + ID).c_str(), &Visible);
			::ImGui::Checkbox(("Casts Shadow ##" + ID).c_str(), &CastsShadow);
			::ImGui::Text("Mesh count: %d", Meshes.size());
			::ImGui::DragFloat3(("Position ##" + ID).c_str(), glm::value_ptr(Position), 0.01f);
			::ImGui::DragFloat3(("Rotation ##" + ID).c_str(), glm::value_ptr(Rotation), 1.0f);
			::ImGui::DragFloat3(("Scale ##" + ID).c_str(), glm::value_ptr(Scale), 0.1f);

			if (::ImGui::CollapsingHeader(("Meshes ##" + ID).c_str()))
			{
				::ImGui::Indent(16.0f);

				for (size_t i{}; i < Meshes.size(); i++)
				{
					Meshes[i].AddToImGui(ID);
				}

				::ImGui::Indent(-16.0f);
			}
		}
	}

	Mesh* Model::GetMesh(std::string_view id)
	{
		for (auto& mesh : Meshes)
		{
			if (mesh.ID == id)
				return &mesh;
		}

		return nullptr;
	}

	AABB Model::GetWorldAABB() const
	{
		std::array<glm::vec3, 8> corners{
		{
			{ BoundingBox.Min.x, BoundingBox.Min.y, BoundingBox.Min.z },
			{ BoundingBox.Max.x, BoundingBox.Min.y, BoundingBox.Min.z },
			{ BoundingBox.Min.x, BoundingBox.Max.y, BoundingBox.Min.z },
			{ BoundingBox.Max.x, BoundingBox.Max.y, BoundingBox.Min.z },

			{ BoundingBox.Min.x, BoundingBox.Min.y, BoundingBox.Max.z },
			{ BoundingBox.Max.x, BoundingBox.Min.y, BoundingBox.Max.z },
			{ BoundingBox.Min.x, BoundingBox.Max.y, BoundingBox.Max.z },
			{ BoundingBox.Max.x, BoundingBox.Max.y, BoundingBox.Max.z }
		} };

		AABB result{};

		result.Min = glm::vec3(FLT_MAX);
		result.Max = glm::vec3(-FLT_MAX);

		for (const glm::vec3& corner : corners)
		{
			glm::vec3 world{ glm::vec3(GetModelMatrix() * glm::vec4(corner, 1.0f)) };

			result.Min = glm::min(result.Min, world);
			result.Max = glm::max(result.Max, world);
		}

		return result;
	}

	bool Model::Intersects(const Model& other) const
	{
		AABB thisBoundingBox{ GetWorldAABB() };
		AABB otherBoundingBox{ other.GetWorldAABB() };

		return
			thisBoundingBox.Min.x <= otherBoundingBox.Max.x &&
			thisBoundingBox.Max.x >= otherBoundingBox.Min.x &&
													  
			thisBoundingBox.Min.y <= otherBoundingBox.Max.y &&
			thisBoundingBox.Max.y >= otherBoundingBox.Min.y &&
													  
			thisBoundingBox.Min.z <= otherBoundingBox.Max.z &&
			thisBoundingBox.Max.z >= otherBoundingBox.Min.z;
	}
}