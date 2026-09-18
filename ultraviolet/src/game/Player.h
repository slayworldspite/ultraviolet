#pragma once

#include <glm/glm.hpp>

namespace ultraviolet
{
	class Player
	{
	public:
		glm::vec3 Position;
		glm::vec3 Velocity;
		float Speed{ 50.0f };
		float MaxSpeed{ 5.0f };
		float JumpHeight{ 12.0f };
		float Height{ 1.8f };
		float Gravity{ 25.0f };
		bool OnGround;
		bool Flying;

	public:
		void UpdateVelocity(double deltaTime);
		void ApplyVelocity(double deltaTime);
		void ApplyFriction(double deltaTime);

		void AddToImGui();
	};
}