#pragma once

#include <SDL.h>
#include <string>

// Forward declarations
class Engine;

/**
 * Base Scene class for the Scene Management System
 * Each scene represents a distinct game state (Main Menu, Gameplay, Pause, etc.)
 */
class Scene {
public:
    virtual ~Scene() = default;

    /**
     * Called when the scene becomes active
     * Use this to initialize scene-specific resources
     */
    virtual void Enter() = 0;

    /**
     * Called when the scene is deactivated
     * Use this to clean up scene-specific resources
     */
    virtual void Exit() = 0;

    /**
     * Handle input events for this scene
     * @param event SDL event to process
     * @param keyboardState Current keyboard state
     */
    virtual void HandleEvent(SDL_Event* event, const Uint8* keyboardState) = 0;

    /**
     * Update scene logic (called every frame)
     */
    virtual void Update() = 0;

    /**
     * Render scene graphics (called every frame)
     */
    virtual void Render() = 0;

    /**
     * Render ImGui UI for this scene (called every frame after Render)
     * Override this method to add ImGui windows specific to this scene
     * Default implementation does nothing
     */
    virtual void RenderImGui() {}

    /**
     * Get the name/identifier of this scene
     * @return Scene name as string
     */
    virtual const std::string& GetName() const = 0;

    /**
     * Set the Engine reference (called by SceneManager)
     */
    void SetEngine(Engine* engine) { _engine = engine; }

protected:
    Engine* _engine = nullptr; // Reference to engine for accessing engine features
};
