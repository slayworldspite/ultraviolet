#include <glm/gtc/matrix_transform.hpp>

#include "ProjectionCamera.h"

namespace ultraviolet
{
	ProjectionCamera::ProjectionCamera() :
		Position{},
		Yaw{ -90 },
		Pitch{},
		FOV{ 90 },
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

	// takes screen size as std::pair because NativeWindow's GetWindowSize() returns a std::pair
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
	}

	void ProjectionCamera::Move(const glm::vec3& vector)
	{
		glm::vec3 right = glm::normalize(glm::cross(m_Front, c_WorldUp));

		Position += right * vector.x;
		Position.y += vector.y;
		Position += m_Front * vector.z;
	}
}