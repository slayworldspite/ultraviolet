#pragma once

#include "Character.h"

namespace ultraviolet
{
	class Player : public Character
	{
	public:
		float Health{ 100.0f };

	public:
		Player();

		void AddToImGui();
	};
}