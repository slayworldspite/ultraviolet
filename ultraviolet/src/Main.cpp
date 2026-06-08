#include "Game.h"
#include "utils/Logger.h"

int main()
{
	ultraviolet::Logger::Initialize("logs");
	
	ultraviolet::Game game;
	game.Run();

	ultraviolet::Logger::Close();

	return 0;
}