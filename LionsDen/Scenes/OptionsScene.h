#pragma once

#include "Scene.h"
#include <vector>
#include <string>
#include <SDL.h>

/**
 * OptionsScene - Dedicated scene for options menu
 * Handles graphics and sound settings
 */
class OptionsScene : public Scene {
public:
    OptionsScene();
    ~OptionsScene() override = default;
    
    void Enter() override;
    void Exit() override;
    void HandleEvent(SDL_Event* event, const Uint8* keyboardState) override;
    void Update() override;
    void Render() override;
    void RenderImGui() override;
    const std::string& GetName() const override;
    
    // State management
    void Show(bool show) { _visible = show; }
    bool IsVisible() const { return _visible; }
    
private:
    static const std::string _name;
    bool _visible = false;
    
    // Resolution selection
    std::vector<SDL_DisplayMode*> _validModes;
    std::vector<std::string> _resolutionStrings;
    int _currentResolutionIndex = 0;
    bool _resolutionsInitialized = false;
    
    // Volume tracking
    int _previousMasterVolume = -1;
    int _previousMusicVolume = -1;
    bool _muteMusic = false;
    
    void DrawGraphicsTab();
    void DrawSoundTab();
    void InitializeResolutions();
    void CleanupResolutions();
};
