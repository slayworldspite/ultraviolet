#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Player.h"
#include "../../renderer/model/ModelLoader.h"

namespace ultraviolet
{
	Player::Player() :
		Character{ "Player" }
	{

	}

	void Player::Update(double deltaTime)
	{
		Character::Update(deltaTime);
	}

	void Player::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Player"))
		{
			::ImGui::Indent(16.0f);

			Character::AddToImGui();

			::ImGui::Indent(-16.0f);
		}
	}
}