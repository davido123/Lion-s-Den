#include "OptionsScene.h"
#include "../Engine.h"
#include "../GUI/Window.h"
#include "../Core/Audio.h"
#include "../Core/Vec2.h"
#include "../Render/Surface.h"
#include "../ImGui/imgui.h"
#include "../Systems/SceneManager.h"
#include "../Systems/UIManager.h"
#include "../Scenes/MainMenuScene.h"
#include "../GUI/ImGuiHelper.h"
#include "../Game/Game.h"
#include <iostream>

const std::string OptionsScene::_name = "Options";

OptionsScene::OptionsScene() 
    : _visible(false)
    , _currentResolutionIndex(0)
    , _resolutionsInitialized(false)
    , _previousMasterVolume(-1)
    , _previousMusicVolume(-1)
    , _muteMusic(false)
{
}

void OptionsScene::Enter() {
    std::cout << "OptionsScene: Entering options menu" << std::endl;
    _visible = true;
    InitializeResolutions();
}

void OptionsScene::Exit() {
    std::cout << "OptionsScene: Exiting options menu" << std::endl;
    _visible = false;
    CleanupResolutions();
}

void OptionsScene::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    // Options scene doesn't handle events directly
    // ESC key is handled in RenderImGui()
}

void OptionsScene::Update() {
    // Options scene doesn't need per-frame updates
}

void OptionsScene::Render() {
    // Options scene doesn't render game graphics
    // All rendering is done via ImGui
}

void OptionsScene::RenderImGui() {
    if (!_visible) return;
    
    // Center window on screen
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImVec2 windowSize = ImVec2(600.0f, 500.0f);
    ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    bool windowOpen = _visible;
    if (ImGui::Begin("Options", &windowOpen, window_flags)) {
        // Handle ESC key to close
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            windowOpen = false;
        }
        
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Graphics")) {
                DrawGraphicsTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Sound")) {
                DrawSoundTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
    
    // Update visibility state
    if (!windowOpen && _visible) {
        _visible = false;
        ImGui::CaptureMouseFromApp(false);
        
        // Notify UIManager that options is closed
        UIManager::GetInstance().ShowOptions(false);
        
        // Notify MainMenuScene that options is closed
        SceneManager& sceneManager = SceneManager::GetInstance();
        Scene* activeScene = sceneManager.GetActiveScene();
        if (activeScene && activeScene->GetName() == "MainMenu") {
            MainMenuScene* mainMenuScene = dynamic_cast<MainMenuScene*>(activeScene);
            if (mainMenuScene) {
                mainMenuScene->SetOptionsVisible(false);
                if (mainMenuScene->GetMainMenuBox()) {
                    mainMenuScene->GetMainMenuBox()->Show(true);
                }
            }
        }
    }
}

void OptionsScene::DrawGraphicsTab() {
    // Display resolution combo
    if (!_resolutionStrings.empty()) {
        const char* combo_preview = _resolutionStrings[_currentResolutionIndex].c_str();
        if (ImGui::BeginCombo("Resolution", combo_preview)) {
            for (int i = 0; i < _resolutionStrings.size(); i++) {
                bool is_selected = (_currentResolutionIndex == i);
                if (ImGui::Selectable(_resolutionStrings[i].c_str(), is_selected)) {
                    _currentResolutionIndex = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        // Fullscreen checkbox
        if (_engine) {
            ImGui::Checkbox("Fullscreen", &_engine->check_fullscreen);
        }
        
        ImGui::Spacing();
        
        // Apply button
        if (ImGui::Button("Apply", ImVec2(120, 0))) {
            if (_engine && _currentResolutionIndex >= 0 && 
                _currentResolutionIndex < _validModes.size()) {
                SDL_DisplayMode* selectedMode = _validModes[_currentResolutionIndex];
                SDL_SetWindowDisplayMode(Window::GetWindow(), selectedMode);
                Window::SetMode(selectedMode->w, selectedMode->h, 
                                _engine->check_fullscreen, "Lion's Den");
                Surface::BeginViewport(Vec2::ZERO, Window::GetCamera()->GetViewport());
                _engine->reschange = true;
                
                // Save settings
                Game::GetInstance().SaveImGuiSettings();
            }
        }
        ImGui::SameLine();
    } else {
        ImGui::Text("No display modes available");
    }
    
    if (ImGui::Button("Back", ImVec2(120, 0))) {
        _visible = false;
    }
}

void OptionsScene::DrawSoundTab() {
    int masterVolume = Audio::GetMasterVolume();
    int musicVolume = Audio::GetMusicVolume();
    
    // Only update if value changed
    if (ImGui::SliderInt("Master Volume", &masterVolume, 0, 100)) {
        if (masterVolume != _previousMasterVolume) {
            Audio::SetMasterVolume(masterVolume);
            _previousMasterVolume = masterVolume;
        }
    }
    
    if (ImGui::SliderInt("Music Volume", &musicVolume, 0, 100)) {
        if (musicVolume != _previousMusicVolume) {
            Audio::SetMusicVolume(musicVolume);
            _previousMusicVolume = musicVolume;
        }
    }
    
    // Update on slider release
    if (ImGui::IsItemDeactivated()) {
        Audio::SetMusicVolume(musicVolume);
        _previousMusicVolume = musicVolume;
    }
    
    ImGui::Checkbox("Mute Music", &_muteMusic);
    if (_muteMusic) {
        Audio::SetMusicVolume(0);
    }
    
    ImGui::Spacing();
    
    // Save settings button
    if (ImGui::Button("Save Settings", ImVec2(120, 0))) {
        Game::GetInstance().SaveImGuiSettings();
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Back", ImVec2(120, 0))) {
        _visible = false;
    }
}

void OptionsScene::InitializeResolutions() {
    if (_resolutionsInitialized) return;
    
    int display_index = 0;
    int modes = SDL_GetNumDisplayModes(display_index);
    
    if (modes <= 0) {
        std::cerr << "OptionsScene: No display modes available" << std::endl;
        return;
    }
    
    _validModes.clear();
    _resolutionStrings.clear();
    
    // Get current window resolution
    int currentWidth = Window::GetWidth();
    int currentHeight = Window::GetHeight();
    
    for (int i = 0; i < modes; i++) {
        SDL_DisplayMode* mode = new SDL_DisplayMode{ SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
        if (SDL_GetDisplayMode(display_index, i, mode) == 0) {
            // Check if duplicate (same width/height)
            bool isDuplicate = false;
            for (auto* existing : _validModes) {
                if (existing->w == mode->w && existing->h == mode->h) {
                    isDuplicate = true;
                    delete mode;
                    break;
                }
            }
            
            if (!isDuplicate) {
                _validModes.push_back(mode);
                std::string resStr = std::to_string(mode->w) + " x " + 
                                    std::to_string(mode->h) + " @ " + 
                                    std::to_string(mode->refresh_rate) + "Hz";
                _resolutionStrings.push_back(resStr);
                
                // Check if current resolution
                if (mode->w == currentWidth && mode->h == currentHeight) {
                    _currentResolutionIndex = _resolutionStrings.size() - 1;
                }
            }
        } else {
            delete mode;
        }
    }
    
    _resolutionsInitialized = true;
}

void OptionsScene::CleanupResolutions() {
    for (auto* mode : _validModes) {
        delete mode;
    }
    _validModes.clear();
    _resolutionStrings.clear();
    _resolutionsInitialized = false;
}

const std::string& OptionsScene::GetName() const {
    return _name;
}
