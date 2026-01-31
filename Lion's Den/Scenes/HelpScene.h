#pragma once

#include "Scene.h"

/**
 * HelpScene - Dedicated scene for help menu
 * Displays game information, controls, mechanics, and credits
 */
class HelpScene : public Scene {
public:
    HelpScene();
    ~HelpScene() override = default;
    
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
    
    void DrawIntroTab();
    void DrawControlsTab();
    void DrawMechanicsTab();
    void DrawCreditsTab();
};
