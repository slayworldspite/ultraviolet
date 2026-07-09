#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Player.h"

namespace ultraviolet
{
	void Player::Update(double deltaTime)
	{
		float dt{ static_cast<float>(deltaTime) };

		if (!Flying)
			Velocity.y -= Gravity * dt;

		Position += Velocity * dt;

		float friction{ glm::pow(0.000025f, dt) };

		Velocity.x *= friction;
		Velocity.z *= friction;

		if (glm::abs(Velocity.x) < 0.1f)
			Velocity.x = 0.0f;

		if (glm::abs(Velocity.z) < 0.1f)
			Velocity.z = 0.0f;

		if (Flying)
			Velocity.y = 0.0f;
	}

	void Player::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Player"))
		{
			::ImGui::Indent(16.0f);

			::ImGui::DragFloat3("Position ##Player", glm::value_ptr(Position), 0.1f);
			::ImGui::DragFloat3("Velocity ##Player", glm::value_ptr(Velocity), 0.1f);
			::ImGui::DragFloat("Speed ##Player", &Speed, 0.1f);
			::ImGui::DragFloat("Jump Height ##Player", &JumpHeight, 0.1f);
			::ImGui::DragFloat("Height ##Player", &Height, 0.1f);
			::ImGui::DragFloat("Gravity ##Player", &Gravity, 0.1f);
			::ImGui::Checkbox("On Ground ##Player", &OnGround);
			::ImGui::Checkbox("Flying ##Player", &Flying);

			::ImGui::Indent(-16.0f);
		}
	}
}