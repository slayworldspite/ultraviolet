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
		CastsShadow{ true }
	{

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
}