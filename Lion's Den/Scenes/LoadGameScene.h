#pragma once

#include "Scene.h"
#include <string>
#include <filesystem>

/**
 * LoadGameScene - Dedicated scene for load game menu
 * Displays save files and allows loading/deleting saves or creating new games
 */
class LoadGameScene : public Scene {
public:
    LoadGameScene();
    ~LoadGameScene() override = default;
    
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
    std::string _savename;
    
    void DrawSaveFileTable();
    void DrawNewGameDialog();
    void RefreshSaveList();
};
