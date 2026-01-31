#include "LoadGameScene.h"
#include "../Engine.h"
#include "../GUI/Window.h"
#include "../Core/Vec2.h"
#include "../ImGui/imgui.h"
#include "../Systems/SceneManager.h"
#include "../Systems/UIManager.h"
#include "../Scenes/MainMenuScene.h"
#include "../Game/Game.h"
#include <iostream>
#include <filesystem>
#include <string>

const std::string LoadGameScene::_name = "LoadGame";

LoadGameScene::LoadGameScene() 
    : _visible(false)
{
}

void LoadGameScene::Enter() {
    std::cout << "LoadGameScene: Entering load game menu" << std::endl;
    _visible = true;
    RefreshSaveList();
}

void LoadGameScene::Exit() {
    std::cout << "LoadGameScene: Exiting load game menu" << std::endl;
    _visible = false;
}

void LoadGameScene::Show(bool show) {
    _visible = show;
    if (show) {
        RefreshSaveList();
    }
}

void LoadGameScene::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    // Load game scene doesn't handle events directly
    // ESC key is handled in RenderImGui()
}

void LoadGameScene::Update() {
    // Refresh save list if needed
    if (_engine && _engine->deleted) {
        RefreshSaveList();
    }
}

void LoadGameScene::Render() {
    // Load game scene doesn't render game graphics
    // All rendering is done via ImGui
}

void LoadGameScene::RenderImGui() {
    if (!_visible) return;
    
    Vec2 MainWindowSize = Window::GetSize();
    Vec2 WindowPos = MainWindowSize/4;
    Vec2 WindowSize = MainWindowSize/2;

    ImVec2 ImWindowPos = ImVec2(WindowPos.x, WindowPos.y);
    ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);

    ImGui::SetNextWindowPos(ImWindowPos);
    ImGui::SetNextWindowSize(ImWindowSize);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;

    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    
    // Track window state
    bool windowOpen = true;
    if (!ImGui::Begin("Load Game", &windowOpen, window_flags)) {
        ImGui::End();
        windowOpen = false;
    }

    DrawSaveFileTable();
    DrawNewGameDialog();
    
    if (ImGui::Button("Back")) {
        windowOpen = false;
    }
    
    // Handle ESC key to close
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        windowOpen = false;
    }
    
    ImGui::End();
    
    // Update visibility state
    if (!windowOpen && _visible) {
        _visible = false;
        ImGui::CaptureMouseFromApp(false);
        
        // Notify UIManager that load game is closed
        UIManager::GetInstance().ShowLoadGame(false);
        
        // Notify MainMenuScene that load game is closed
        SceneManager& sceneManager = SceneManager::GetInstance();
        Scene* activeScene = sceneManager.GetActiveScene();
        if (activeScene && activeScene->GetName() == "MainMenu") {
            MainMenuScene* mainMenuScene = dynamic_cast<MainMenuScene*>(activeScene);
            if (mainMenuScene) {
                mainMenuScene->SetLoadVisible(false);
                if (mainMenuScene->GetMainMenuBox()) {
                    mainMenuScene->GetMainMenuBox()->Show(true);
                }
            }
        }
    }
}

void LoadGameScene::DrawSaveFileTable() {
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    
    if (ImGui::BeginTable("Saves", 4, flags)) {
        ImGui::TableSetupColumn("NR");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        int rows = _engine ? _engine->saves.size() : 0;

        for (int row = 0; row < rows; row++) {
            ImGui::TableNextRow();

            for (int column = 0; column < 4; column++) {
                if (column == 0) {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%d", row+1);
                }
                if (column == 1) {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("%s", _engine->saves.at(row).c_str());
                }
                if (column == 2) {
                    ImGui::TableSetColumnIndex(column);
                    // Get player time from Game singleton if available
                    Player* player = Game::GetInstance().GetPlayer();
                    if (player) {
                        ImGui::Text("Time:%d ms", player->timeplayed);
                    } else {
                        ImGui::Text("Time: N/A");
                    }
                }
                if (column == 3) {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::PushID(row);
                    if (ImGui::Button("Load Game")) {
                        _savename = _engine->saves.at(row);
                        Game::GetInstance().SetSavename(_savename);
                        Game::GetInstance().LoadGame();
                        if (_engine) {
                            _engine->LoadedSaveNr = row;
                        }
                    }
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(row + 1000);
                    if (ImGui::Button("Delete")) {
                        _savename = _engine->saves.at(row);
                        std::filesystem::remove_all("Data/Saves/"+_savename);
                        if (_engine) {
                            _engine->deleted = true;
                        }
                    }
                    ImGui::PopID();
                }
            }
        }
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", rows+1);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Empty Slot");

        ImGui::TableSetColumnIndex(3);
        if (ImGui::Button("New Game")) {
            if (_engine) {
                _engine->LoadedSaveNr = rows + 1;
            }
            // Start new game directly with default name so one click starts the game
            std::string defaultName = "save";
            if (rows > 0) {
                defaultName = "save_" + std::to_string(rows + 1);
            }
            Game::GetInstance().NewGame(defaultName);
            _visible = false;
            UIManager::GetInstance().ShowLoadGame(false);
        }
        ImGui::EndTable();
    }
}

void LoadGameScene::DrawNewGameDialog() {
    ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("New Game", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Please specify a name for a save file");
        ImGui::Separator();

        static char name[32] = "save";
        ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));

        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            Game::GetInstance().NewGame(name);
            ImGui::CloseCurrentPopup();
            _visible = false;
            UIManager::GetInstance().ShowLoadGame(false);
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

void LoadGameScene::RefreshSaveList() {
    if (!_engine) return;

    std::filesystem::create_directories("Data/Saves");
    _engine->saves.clear();
    try {
        for (const auto& file : std::filesystem::directory_iterator("Data/Saves/")) {
            if (file.is_directory()) {
                std::string savename = file.path().filename().string();
                _engine->saves.push_back(savename);
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Directory missing or unreadable; leave saves empty
    }
    _engine->deleted = false;
}

const std::string& LoadGameScene::GetName() const {
    return _name;
}
