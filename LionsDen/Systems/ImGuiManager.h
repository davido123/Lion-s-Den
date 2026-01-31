#pragma once

#include <string>
#include <SDL.h>
#include "../ImGui/imgui.h"

/**
 * ImGuiManager - Centralized ImGui management system
 * Handles initialization, frame lifecycle, styling, and settings persistence
 */
class ImGuiManager {
public:
    static ImGuiManager& GetInstance();
    
    // Initialization and cleanup
    void Initialize(SDL_Renderer* renderer, int width, int height);
    void Shutdown();
    bool IsInitialized() const { return _initialized; }
    
    // Frame lifecycle
    void BeginFrame();
    void EndFrame();
    void Render();
    
    // Style management
    void ApplyCustomStyle();
    void LoadStyleFromFile(const std::string& path);
    void SaveStyleToFile(const std::string& path);
    
    // Window helpers
    bool BeginCenteredWindow(const char* name, bool* open, ImGuiWindowFlags flags = 0);
    bool BeginModalDialog(const char* name, bool* open, ImVec2 size = ImVec2(0, 0));
    
    // Settings persistence
    void SaveSettings();
    void LoadSettings();
    
    // Configuration
    void SetIniFilePath(const std::string& path) { _iniFilePath = path; }
    const std::string& GetIniFilePath() const { return _iniFilePath; }

private:
    ImGuiManager() = default;
    ~ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
    
    bool _initialized = false;
    std::string _iniFilePath = "imgui.ini";
    SDL_Renderer* _renderer = nullptr;
};
