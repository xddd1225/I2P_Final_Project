// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/HomeScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/HistoryScene.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

	game.AddNewScene("home", new HomeScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("history", new HistoryScene());

	game.Start("home", 60, 1600, 832);
	return 0;
}