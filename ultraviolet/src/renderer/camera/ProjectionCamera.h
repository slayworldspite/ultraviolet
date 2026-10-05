#pragma once

#include <glm/glm.hpp>

#include "../Shader.h"
#include "../objects/UniformBuffer.h"

namespace ultraviolet
{
	class ProjectionCamera
	{
	public:
		glm::vec3 Position;
		float Yaw;
		float Pitch;
		float Roll;
		float FOV;

	public:
		ProjectionCamera();

		void Update();
		void Apply(std::pair<int, int> size);
		glm::vec3 GetFront() const;
		glm::vec3 GetRight() const;
		void AddToImGui();
		void AddToImGui(const std::string& id);

	private:
		std::unique_ptr<UniformBuffer> m_UniformBuffer;
		const glm::vec3 c_WorldUp{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Front;
	};
}