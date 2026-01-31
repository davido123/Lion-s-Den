#pragma once

#include <string>

// Forward declarations
class SceneManager;
class OptionsScene;
class HelpScene;
class LoadGameScene;

/**
 * UIManager - Centralized UI state management
 * Handles menu visibility and scene switching for UI-related scenes
 */
class UIManager {
public:
    static UIManager& GetInstance();
    
    // Menu visibility control
    void ShowOptions(bool show);
    bool IsOptionsVisible() const { return _showOptions; }
    
    void ShowHelp(bool show);
    bool IsHelpVisible() const { return _showHelp; }
    
    void ShowLoadGame(bool show);
    bool IsLoadGameVisible() const { return _showLoadGame; }
    
    // Convenience methods for opening/closing menus
    void OpenOptions();
    void OpenHelp();
    void OpenLoadGame();
    void CloseAllMenus();
    
    // Event handling (for ESC key, etc.)
    void HandleEvent(void* event); // SDL_Event* - using void* to avoid SDL dependency in header
    
    // Initialization
    void Initialize();
    void Shutdown();

private:
    UIManager() = default;
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    
    bool _showOptions = false;
    bool _showHelp = false;
    bool _showLoadGame = false;
    
    bool _initialized = false;
    
    // Helper methods
    OptionsScene* GetOptionsScene();
    HelpScene* GetHelpScene();
    LoadGameScene* GetLoadGameScene();
};
