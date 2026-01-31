/*!
 boldtemple Gaming ©, http://gaming.boldtemple.net
 OpenSource Project - Check Development, License & Readme Sections.

 BGE - boldtemple Cross Platform Game Engine
 /main.cpp
 !*/
#include "Engine.h"
#include "Game/Game.h"
#include "Systems/SceneManager.h"
#include "Systems/UIManager.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GameScene.h"
#include "Scenes/OptionsScene.h"
#include "Scenes/HelpScene.h"
#include "Scenes/LoadGameScene.h"

#undef main

#ifndef BUILD_STATIC_LIB

bool Engine::Pause = 0;

void Engine::OnInit() {
	Game::GetInstance().SetEngine(this);
	Game::GetInstance().OnGameInit();
	
	// Initialize Scene Management System
	// Register scenes with SceneManager
	SceneManager& sceneManager = SceneManager::GetInstance();
	
	// Create and register scenes
	auto mainMenuScene = std::make_unique<MainMenuScene>();
	mainMenuScene->SetEngine(this);
	sceneManager.RegisterScene(std::move(mainMenuScene));
	
	auto gameScene = std::make_unique<GameScene>();
	gameScene->SetEngine(this);
	sceneManager.RegisterScene(std::move(gameScene));
	
	// Register UI scenes (Options, Help, LoadGame)
	auto optionsScene = std::make_unique<OptionsScene>();
	optionsScene->SetEngine(this);
	sceneManager.RegisterScene(std::move(optionsScene));
	
	auto helpScene = std::make_unique<HelpScene>();
	helpScene->SetEngine(this);
	sceneManager.RegisterScene(std::move(helpScene));
	
	auto loadGameScene = std::make_unique<LoadGameScene>();
	loadGameScene->SetEngine(this);
	sceneManager.RegisterScene(std::move(loadGameScene));
	
	// Initialize UIManager
	UIManager::GetInstance().Initialize();
	
	// Set initial scene to main menu
	sceneManager.SetActiveScene("MainMenu");
	
	// Note: For now, we're keeping Game class integration
	// SceneManager is set up but not yet fully integrated
	// This allows gradual migration
}

void Engine::OnEvent(SDL_Event *event, const Uint8 *keyboardState) {
	Game::GetInstance().OnGameEvent(event, keyboardState);
}

void Engine::OnUpdate() {
	Game::GetInstance().OnGameUpdate();
}

void Engine::OnRender() {
	Game::GetInstance().OnGameRender();
}

void Engine::OnRenderDebugGui() {
	Game::GetInstance().OnRenderDebugGui();
}

void Engine::SaveGame() {
	Game::GetInstance().SaveGame();
}

void Engine::LoadGame() {
	Game::GetInstance().LoadGame();
}

// DrawOptionsMenu, DrawLoadMenu, DrawHelpMenu removed - now handled by scenes

void Engine::OnCleanUp() {
	Game::GetInstance().OnGameCleanup();
	
	// Shutdown UIManager
	UIManager::GetInstance().Shutdown();
	
	// Cleanup Scene Management System
	SceneManager::GetInstance().Cleanup();
}

void Engine::RepositionGuiAtResChange() {
	Game::GetInstance().RepositionGuiAtResChange();
}

int main() {
	Engine engine;
	engine.Start();
	return 0;
}
#endif
