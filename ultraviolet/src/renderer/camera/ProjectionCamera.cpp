#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "ProjectionCamera.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	ProjectionCamera::ProjectionCamera() :
		Position{},
		Yaw{},
		Pitch{},
		Roll{},
		FOV{ 75 },
		m_Front{}
	{
		Update();
	}

	void ProjectionCamera::Update()
	{
		glm::vec3 direction{};

		float yaw{ glm::radians(Yaw) };
		float pitch{ glm::radians(Pitch) };

		direction.x = cos(yaw) * cos(pitch);
		direction.y = sin(pitch);
		direction.z = sin(yaw) * cos(pitch);

		m_Front = glm::normalize(direction);
	}

	glm::vec3 ProjectionCamera::GetFront() const
	{
		return m_Front;
	}

	glm::vec3 ProjectionCamera::GetForward() const
	{
		glm::vec3 forward{ m_Front };
		forward.y = 0;
		return glm::normalize(forward);
	}

	glm::vec3 ProjectionCamera::GetRight() const
	{
		return glm::normalize(glm::cross(m_Front, c_WorldUp));
	}

	glm::vec3 ProjectionCamera::GetWorldUp() const
	{
		return c_WorldUp;
	}

	void ProjectionCamera::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Camera"))
		{
			::ImGui::Indent(16.0f);

			::ImGui::DragFloat3("Position ##camera", glm::value_ptr(Position));
			::ImGui::DragFloat("Yaw ##camera", &Yaw);
			::ImGui::DragFloat("Pitch ##camera", &Pitch);
			::ImGui::DragFloat("Roll ##camera", &Roll);
			::ImGui::DragFloat("FOV ##camera", &FOV);

			::ImGui::Indent(-16.0f);
		}
	}

	void ProjectionCamera::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("Camera " + id).c_str()))
		{
			::ImGui::DragFloat3(("Position ##" + id).c_str(), glm::value_ptr(Position), 0.1f);
			::ImGui::DragFloat(("Yaw ##" + id).c_str(), &Yaw);
			::ImGui::DragFloat(("Pitch ##" + id).c_str(), &Pitch);
			::ImGui::DragFloat(("Roll ##" + id).c_str(), &Roll);
			::ImGui::DragFloat(("FOV ##" + id).c_str(), &FOV);
		}
	}
}