#include "SceneManager.h"
#include "../Engine.h"
#include <iostream>
#include <memory>

SceneManager& SceneManager::GetInstance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::RegisterScene(std::unique_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "SceneManager: Attempted to register null scene" << std::endl;
        return;
    }

    std::string name = scene->GetName();
    if (_scenes.find(name) != _scenes.end()) {
        std::cerr << "SceneManager: Scene '" << name << "' already registered, replacing..." << std::endl;
    }

    scene->SetEngine(nullptr); // Will be set when scene becomes active
    _scenes[name] = std::move(scene);
    std::cout << "SceneManager: Registered scene '" << name << "'" << std::endl;
}

bool SceneManager::SetActiveScene(const std::string& name) {
    auto it = _scenes.find(name);
    if (it == _scenes.end()) {
        std::cerr << "SceneManager: Scene '" << name << "' not found" << std::endl;
        return false;
    }

    // Exit current scene
    if (_currentScene) {
        _currentScene->Exit();
    }

    // Set new active scene
    _currentScene = it->second.get();
    _currentSceneName = name;

    // Note: Engine reference should be set when scenes are registered
    // It's set in main.cpp during initialization

    // Enter new scene
    _currentScene->Enter();

    std::cout << "SceneManager: Switched to scene '" << name << "'" << std::endl;
    return true;
}

Scene* SceneManager::GetScene(const std::string& name) const {
    auto it = _scenes.find(name);
    if (it != _scenes.end()) {
        return it->second.get();
    }
    return nullptr;
}

void SceneManager::Update() {
    if (_currentScene) {
        _currentScene->Update();
    }
}

void SceneManager::Render() {
    if (_currentScene) {
        _currentScene->Render();
    }
}

void SceneManager::RenderImGui() {
    // Render ImGui for all scenes, not just the active one
    // This allows overlay scenes (Options, Help, LoadGame) to render on top
    for (const auto& pair : _scenes) {
        Scene* scene = pair.second.get();
        if (scene) {
            scene->RenderImGui();
        }
    }
}

void SceneManager::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    if (_currentScene) {
        _currentScene->HandleEvent(event, keyboardState);
    }
}

void SceneManager::Cleanup() {
    if (_currentScene) {
        _currentScene->Exit();
        _currentScene = nullptr;
    }
    _scenes.clear();
    _currentSceneName.clear();
}
