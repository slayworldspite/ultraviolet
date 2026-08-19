#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "ProjectionCamera.h"
#include "../../utils/Logger.h"

namespace
{
	struct CameraData
	{
		glm::mat4 Projection;
		glm::mat4 View;
		glm::vec3 Position;
	};
}

namespace ultraviolet
{
	ProjectionCamera::ProjectionCamera() :
		Position{},
		Yaw{},
		Pitch{},
		Roll{},
		FOV{ 75 },
		m_Front{},
		m_UniformBuffer{ nullptr }
	{
		Update();

		CameraData data{};

		m_UniformBuffer = std::make_unique<UniformBuffer>(&data, sizeof(data), 0);
	}

	void ProjectionCamera::Update()
	{
		glm::vec3 direction{};

		direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		direction.y = sin(glm::radians(Pitch));
		direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		m_Front = glm::normalize(direction);
	}

	void ProjectionCamera::Apply(std::pair<int, int> size)
	{
		glm::mat4 roll{ glm::rotate(glm::mat4{ 1.0f }, glm::radians(-Roll), m_Front) };

		CameraData data{
			.Projection{
				glm::perspective(
					glm::radians(FOV),
					static_cast<float>(size.first) / static_cast<float>(size.second),
					0.1f,
					100.0f
				)
			},
			.View{
				glm::lookAt(
					Position, 
					Position + m_Front, 
					c_WorldUp * glm::mat3{ roll })
			},
			.Position{ Position }
		};

		m_UniformBuffer->SetData(&data, sizeof(data));
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