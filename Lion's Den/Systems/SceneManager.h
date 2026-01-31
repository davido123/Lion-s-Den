#pragma once

#include <string>
#include <map>
#include <memory>
#include "../Scenes/Scene.h"

/**
 * SceneManager - Singleton class for managing game scenes
 * Handles scene registration, switching, and lifecycle management
 */
class SceneManager {
public:
    /**
     * Get the singleton instance
     */
    static SceneManager& GetInstance();

    /**
     * Register a scene with the manager
     * @param scene Unique pointer to the scene (ownership transferred)
     */
    void RegisterScene(std::unique_ptr<Scene> scene);

    /**
     * Set the active scene by name
     * Calls Exit() on current scene and Enter() on new scene
     * @param name Name of the scene to activate
     * @return true if scene was found and activated, false otherwise
     */
    bool SetActiveScene(const std::string& name);

    /**
     * Get the currently active scene
     * @return Pointer to active scene, or nullptr if none
     */
    Scene* GetActiveScene() const { return _currentScene; }

    /**
     * Get a scene by name without activating it
     * @param name Name of the scene
     * @return Pointer to scene, or nullptr if not found
     */
    Scene* GetScene(const std::string& name) const;

    /**
     * Update the active scene
     * Should be called from Engine::OnUpdate()
     */
    void Update();

    /**
     * Render the active scene
     * Should be called from Engine::OnRender()
     */
    void Render();

    /**
     * Render ImGui UI for the active scene
     * Should be called from Engine::Core_Render() after Render()
     */
    void RenderImGui();

    /**
     * Handle events for the active scene
     * Should be called from Engine::OnEvent()
     * @param event SDL event to process
     * @param keyboardState Current keyboard state
     */
    void HandleEvent(SDL_Event* event, const Uint8* keyboardState);

    /**
     * Clean up all scenes
     * Should be called during engine shutdown
     */
    void Cleanup();

    // Delete copy constructor and assignment operator (singleton pattern)
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    SceneManager() = default;
    ~SceneManager() = default;

    std::map<std::string, std::unique_ptr<Scene>> _scenes;
    Scene* _currentScene = nullptr;
    std::string _currentSceneName;
};
