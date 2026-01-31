#include "MainMenuScene.h"
#include "../Engine.h"
#include "../GUI/GUI.h"
#include "../IO/Mouse.h"
#include "../ImGui/imgui.h"
#include "../Systems/SceneManager.h"
#include "../Systems/UIManager.h"
#include "../Core/Resources.h"
#include "../Core/Vec2.h"
#include <iostream>

const std::string MainMenuScene::_name = "MainMenu";

MainMenuScene::MainMenuScene() 
    : _mainMenuBox(nullptr)
    , _buttonNewGame(nullptr)
    , _buttonLoadGame(nullptr)
    , _buttonOptions(nullptr)
    , _buttonHelp(nullptr)
    , _buttonExit(nullptr)
    , _showOptions(false)
    , _showLoad(false)
    , _showHelp(false)
    , _layerGui(nullptr)
{
}

void MainMenuScene::Enter() {
    std::cout << "MainMenuScene: Entering main menu" << std::endl;
    
    // Get GUI layer from engine
    if (_engine) {
        _layerGui = _engine->GetRootAtLayer(1); // GUI layer
    }
    
    SetupMainMenu();
}

void MainMenuScene::Exit() {
    std::cout << "MainMenuScene: Exiting main menu" << std::endl;
    // Hide menu when exiting
    if (_mainMenuBox) {
        _mainMenuBox->Show(false);
    }
    // Note: We don't delete GUI elements here as they're managed by Game class during migration
}

void MainMenuScene::SetupMainMenu() {
    if (!_layerGui) {
        std::cerr << "MainMenuScene: GUI layer not available" << std::endl;
        return;
    }
    
    // Re-entering scene: reuse existing UI to avoid leaks and duplicate buttons
    if (_mainMenuBox) {
        _mainMenuBox->Show(true);
        _mainMenuBox->RecalcCentered();
        std::cout << "MainMenuScene: Reusing existing menu" << std::endl;
        return;
    }
    
    // Create main menu box
    _mainMenuBox = new Box(85, 75, 20, 40, Resources::GetDefaultFont(), 16);
    _mainMenuBox->SetLabel("MainMenu");
    _layerGui->Connect(_mainMenuBox);
    
    // Create menu buttons
    _buttonNewGame = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Play");
    _mainMenuBox->Connect(_buttonNewGame);
    
    _buttonLoadGame = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Load Game");
    _mainMenuBox->Connect(_buttonLoadGame);
    
    _buttonHelp = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Help");
    _mainMenuBox->Connect(_buttonHelp);
    
    _buttonOptions = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Options");
    _mainMenuBox->Connect(_buttonOptions);
    
    _buttonExit = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Quit Game");
    _mainMenuBox->Connect(_buttonExit);
    
    // Center the menu
    _mainMenuBox->RecalcCentered();
    
    std::cout << "MainMenuScene: Main menu setup complete" << std::endl;
}

void MainMenuScene::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    if (!event) return;

    // Sync state with UIManager
    UIManager& uiManager = UIManager::GetInstance();
    _showOptions = uiManager.IsOptionsVisible();
    _showLoad = uiManager.IsLoadGameVisible();
    _showHelp = uiManager.IsHelpVisible();

    // Only handle events if options/load/help menus are not showing
    // This prevents the menu from being unresponsive when these dialogs are open
    if (_showOptions || _showLoad || _showHelp) {
        // Ensure menu box is hidden when dialogs are showing
        if (_mainMenuBox && _mainMenuBox->IsVisible()) {
            _mainMenuBox->Show(false);
        }
        return;
    }

    // Ensure menu box is visible and shown when no dialogs are open
    if (_mainMenuBox && !_mainMenuBox->IsVisible()) {
        _mainMenuBox->Show(true);
    }

    // Let the GUI system handle clicks through its normal update cycle
    // We'll check ImGui capture in HandleButtonClicks if needed
    if (event->type == SDL_MOUSEBUTTONDOWN && Mouse::Pressed(MOUSE_LEFT)) {
        HandleButtonClicks();
    }
}

void MainMenuScene::HandleButtonClicks() {
    // Don't check WantCaptureMouse here because it's from the previous frame
    // Instead, we rely on the _showOptions/_showLoad/_showHelp checks in HandleEvent()
    // which prevent this function from being called when ImGui menus are open
    
    if (GUI::GetLastClicked() == nullptr) return;
    
    const std::string& clickedLabel = GUI::GetLastClicked()->GetLabel();
    
    if (clickedLabel == "Quit Game") {
        if (_engine) {
            _engine->Stop();
        }
    }
    else if (clickedLabel == "Options") {
        if (_mainMenuBox) {
            _mainMenuBox->Show(false);
        }
        _showOptions = true;
        UIManager::GetInstance().OpenOptions();
    }
    else if (clickedLabel == "Play") {
        if (_mainMenuBox) {
            _mainMenuBox->Show(false);
        }
        _showLoad = true;
        UIManager::GetInstance().OpenLoadGame();
    }
    else if (clickedLabel == "Load Game") {
        if (_mainMenuBox) {
            _mainMenuBox->Show(false);
        }
        _showLoad = true;
        UIManager::GetInstance().OpenLoadGame();
    }
    else if (clickedLabel == "Help") {
        if (_mainMenuBox) {
            _mainMenuBox->Show(false);
        }
        _showHelp = true;
        UIManager::GetInstance().OpenHelp();
    }
}

void MainMenuScene::Update() {
    // Update menu animations, timers, etc.
}

void MainMenuScene::Render() {
    // Menu rendering is handled by the GUI system through layers
    // The menu box and buttons are connected to the GUI layer
    // and will be rendered automatically by Engine::Core_Render()
    // This method can be used for scene-specific rendering if needed
}
