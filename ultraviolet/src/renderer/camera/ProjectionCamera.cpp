#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "ProjectionCamera.h"

namespace ultraviolet
{
	ProjectionCamera::ProjectionCamera() :
		Position{},
		Yaw{ -90 },
		Pitch{},
		FOV{ 75 },
		m_Front{}
	{
		Update();
	}

	void ProjectionCamera::Update()
	{
		glm::vec3 direction{};

		direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		direction.y = sin(glm::radians(Pitch));
		direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		m_Front = glm::normalize(direction);
	}

	void ProjectionCamera::Apply(const Shader& shader, std::pair<int, int> size) const
	{
		glm::mat4 projection{ glm::perspective(
			glm::radians(FOV),
			(float)size.first / (float)size.second,
			0.1f,
			100.0f
		) };

		glm::mat4 view{ glm::lookAt(
			Position,
			Position + m_Front,
			c_WorldUp
		) };

		shader.Use();

		shader.Set("u_Projection", projection);
		shader.Set("u_View", view);
		shader.Set("u_CameraPosition", Position);
	}

	void ProjectionCamera::Move(const glm::vec3& vector)
	{
		glm::vec3 right = glm::normalize(glm::cross(m_Front, c_WorldUp));

		Position += right * vector.x;
		Position.y += vector.y;
		Position += m_Front * vector.z;
	}

	void ProjectionCamera::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Camera"))
		{
			::ImGui::DragFloat3("Position", glm::value_ptr(Position));
			::ImGui::DragFloat("Yaw", &Yaw);
			::ImGui::DragFloat("Pitch", &Pitch);
			::ImGui::DragFloat("FOV", &FOV);
		}
	}

	void ProjectionCamera::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("Camera " + id).c_str()))
		{
			::ImGui::DragFloat3(("Position ##" + id).c_str(), glm::value_ptr(Position));
			::ImGui::DragFloat(("Yaw ##" + id).c_str(), &Yaw);
			::ImGui::DragFloat(("Pitch ##" + id).c_str(), &Pitch);
			::ImGui::DragFloat(("FOV ##" + id).c_str(), &FOV);
		}
	}
}