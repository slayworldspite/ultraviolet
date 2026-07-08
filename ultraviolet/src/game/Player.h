#pragma once

#include <glm/glm.hpp>

namespace ultraviolet
{
	class Player
	{
	public:
		glm::vec3 Position;
		glm::vec3 Velocity;
		float Speed{ 100.0f };
		float JumpHeight{ 10.0f };
		float Height{ 1.8f };
		bool OnGround;
		bool Flying;

	public:
		void Update(double deltaTime);

		void AddToImGui();
	};
}