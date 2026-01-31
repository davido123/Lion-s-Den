#pragma once

#include "Scene.h"
#include "../GUI/Widgets/Box.h"
#include "../GUI/Widgets/Button.h"
#include "../Core/Object.h"

/**
 * MainMenuScene - Handles the main menu state
 * Manages menu UI, button interactions, and transitions to other scenes
 */
class MainMenuScene : public Scene {
public:
    MainMenuScene();
    ~MainMenuScene() override = default;

    void Enter() override;
    void Exit() override;
    void HandleEvent(SDL_Event* event, const Uint8* keyboardState) override;
    void Update() override;
    void Render() override;
    const std::string& GetName() const override { return _name; }

    // Access to menu elements (for Game class compatibility during migration)
    Box* GetMainMenuBox() const { return _mainMenuBox; }
    bool IsOptionsVisible() const { return _showOptions; }
    bool IsLoadVisible() const { return _showLoad; }
    bool IsHelpVisible() const { return _showHelp; }
    void SetOptionsVisible(bool visible) { _showOptions = visible; }
    void SetLoadVisible(bool visible) { _showLoad = visible; }
    void SetHelpVisible(bool visible) { _showHelp = visible; }

private:
    static const std::string _name;
    
    // GUI Elements
    Box* _mainMenuBox;
    Button* _buttonNewGame;
    Button* _buttonLoadGame;
    Button* _buttonOptions;
    Button* _buttonHelp;
    Button* _buttonExit;
    
    // UI state flags
    bool _showOptions;
    bool _showLoad;
    bool _showHelp;
    
    // Layer reference
    Object* _layerGui;
    
    void SetupMainMenu();
    void HandleButtonClicks();
};
