#include "Game.h"

namespace ultraviolet
{
	Game::Game() :
		GameWindow(
			NativeWindowSettings{
				.Title{ "ultraviolet" },
				.Width{ 1280 },
				.Height{ 720 }
			},
			GameWindowSettings{
				.UpdateFrequency{ 60.0f }
			}
		)
	{

	}
}