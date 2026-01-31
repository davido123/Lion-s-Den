#include "UIManager.h"
#include "../Systems/SceneManager.h"
#include "../Scenes/OptionsScene.h"
#include "../Scenes/HelpScene.h"
#include "../Scenes/LoadGameScene.h"
#include <SDL.h>
#include <iostream>

UIManager& UIManager::GetInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::Initialize() {
    if (_initialized) {
        std::cerr << "UIManager: Already initialized" << std::endl;
        return;
    }
    
    _initialized = true;
    std::cout << "UIManager: Initialized" << std::endl;
}

void UIManager::Shutdown() {
    if (!_initialized) return;
    
    CloseAllMenus();
    _initialized = false;
    std::cout << "UIManager: Shutdown" << std::endl;
}

void UIManager::ShowOptions(bool show) {
    _showOptions = show;
    OptionsScene* scene = GetOptionsScene();
    if (scene) {
        scene->Show(show);
    }
}

void UIManager::ShowHelp(bool show) {
    _showHelp = show;
    HelpScene* scene = GetHelpScene();
    if (scene) {
        scene->Show(show);
    }
}

void UIManager::ShowLoadGame(bool show) {
    _showLoadGame = show;
    LoadGameScene* scene = GetLoadGameScene();
    if (scene) {
        scene->Show(show);
    }
}

void UIManager::OpenOptions() {
    ShowOptions(true);
}

void UIManager::OpenHelp() {
    ShowHelp(true);
}

void UIManager::OpenLoadGame() {
    ShowLoadGame(true);
}

void UIManager::CloseAllMenus() {
    ShowOptions(false);
    ShowHelp(false);
    ShowLoadGame(false);
}

void UIManager::HandleEvent(void* event) {
    SDL_Event* sdlEvent = static_cast<SDL_Event*>(event);
    if (!sdlEvent) return;
    
    // Handle ESC key to close menus
    if (sdlEvent->type == SDL_KEYDOWN && sdlEvent->key.keysym.sym == SDLK_ESCAPE) {
        if (_showOptions || _showHelp || _showLoadGame) {
            CloseAllMenus();
        }
    }
}

OptionsScene* UIManager::GetOptionsScene() {
    SceneManager& sm = SceneManager::GetInstance();
    Scene* scene = sm.GetScene("Options");
    return dynamic_cast<OptionsScene*>(scene);
}

HelpScene* UIManager::GetHelpScene() {
    SceneManager& sm = SceneManager::GetInstance();
    Scene* scene = sm.GetScene("Help");
    return dynamic_cast<HelpScene*>(scene);
}

LoadGameScene* UIManager::GetLoadGameScene() {
    SceneManager& sm = SceneManager::GetInstance();
    Scene* scene = sm.GetScene("LoadGame");
    return dynamic_cast<LoadGameScene*>(scene);
}
