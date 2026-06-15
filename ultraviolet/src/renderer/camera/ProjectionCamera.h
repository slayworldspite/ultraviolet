#pragma once

#include <glm/glm.hpp>

#include "../Shader.h"

namespace ultraviolet
{
	class ProjectionCamera
	{
	public:
		glm::vec3 Position;
		float Yaw;
		float Pitch;
		float FOV;

	public:
		ProjectionCamera();

		void Update();
		void Apply(const Shader& shader, std::pair<int, int> size) const;
		void Move(const glm::vec3& vector);
		void AddToImGui();
		void AddToImGui(const std::string& id);

	private:
		const glm::vec3 c_WorldUp{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Front;
	};
}