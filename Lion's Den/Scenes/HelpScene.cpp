#include "HelpScene.h"
#include "../Engine.h"
#include "../GUI/Window.h"
#include "../Core/Vec2.h"
#include "../ImGui/imgui.h"
#include "../Systems/SceneManager.h"
#include "../Systems/UIManager.h"
#include "../Scenes/MainMenuScene.h"
#include <iostream>

const std::string HelpScene::_name = "Help";

HelpScene::HelpScene() 
    : _visible(false)
{
}

void HelpScene::Enter() {
    std::cout << "HelpScene: Entering help menu" << std::endl;
    _visible = true;
}

void HelpScene::Exit() {
    std::cout << "HelpScene: Exiting help menu" << std::endl;
    _visible = false;
}

void HelpScene::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    // Help scene doesn't handle events directly
    // ESC key is handled in RenderImGui()
}

void HelpScene::Update() {
    // Help scene doesn't need per-frame updates
}

void HelpScene::Render() {
    // Help scene doesn't render game graphics
    // All rendering is done via ImGui
}

void HelpScene::RenderImGui() {
    if (!_visible) return;
    
    Vec2 MainWindowSize = Window::GetSize();
    Vec2 WindowPos = MainWindowSize/4;
    Vec2 WindowSize = MainWindowSize/2;

    ImVec2 ImWindowPos = ImVec2(WindowPos.x, WindowPos.y);
    ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);

    ImGui::SetNextWindowPos(ImWindowPos);
    ImGui::SetNextWindowSize(ImWindowSize);

    ImGuiWindowFlags window_flags = 0;
    
    // Track window state
    bool windowOpen = true;
    if (!ImGui::Begin("Help", &windowOpen, window_flags)) {
        ImGui::End();
        windowOpen = false;
    }

    ImGui::BeginTabBar("Bar");
    if (ImGui::BeginTabItem("Intro")) {
        DrawIntroTab();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Sterowanie")) {
        DrawControlsTab();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Mechaniki Gry")) {
        DrawMechanicsTab();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Credits")) {
        DrawCreditsTab();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();

    ImGui::Separator();
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
        
        // Notify UIManager that help is closed
        UIManager::GetInstance().ShowHelp(false);
        
        // Notify MainMenuScene that help is closed
        SceneManager& sceneManager = SceneManager::GetInstance();
        Scene* activeScene = sceneManager.GetActiveScene();
        if (activeScene && activeScene->GetName() == "MainMenu") {
            MainMenuScene* mainMenuScene = dynamic_cast<MainMenuScene*>(activeScene);
            if (mainMenuScene) {
                mainMenuScene->SetHelpVisible(false);
                if (mainMenuScene->GetMainMenuBox()) {
                    mainMenuScene->GetMainMenuBox()->Show(true);
                }
            }
        }
    }
}

void HelpScene::DrawIntroTab() {
    ImVec2 childSize = ImVec2(0, ImGui::GetContentRegionAvail().y * 0.7f);
    ImGui::BeginChild("child", childSize, true);
    ImGui::TextWrapped("Witaj w grze Lion's Den\n\nLion's Den jest gra hack&slash z elementami RPG\nTa gra jest projektem stworzonym w celu doskonalenia wlasnych umiejetnosci programistycznych\nGra powstala glownie przy uzyciu jezyka C++ oraz Biblioteki SDL2\nAby dowiedziec sie jak grac sprawdz powyzsze zakladki");
    ImGui::EndChild();
}

void HelpScene::DrawControlsTab() {
    ImVec2 childSize = ImVec2(0, ImGui::GetContentRegionAvail().y * 0.7f);
    ImGui::BeginChild("child", childSize, true);
    ImGui::TextWrapped("Poruszanie sie: Klawisze AWSD \nAtakowanie: PPM\nBieganie: LShift\nEkwipunek: I\nStatystyki postaci: C");
    ImGui::EndChild();
}

void HelpScene::DrawMechanicsTab() {
    ImVec2 childSize = ImVec2(0, ImGui::GetContentRegionAvail().y * 0.7f);
    ImGui::BeginChild("child", childSize, true);
    ImGui::TextWrapped("Rozwoj postaci polega na zdobywaniu przedmiotow z pokonanych przeciwnikow\nPrzedmioty jakie mozemy spotkac w grze to:\nHelmy, Zbroje, Rekawiczki, Buty, Pierscienie, Amulety, Pasy, Miecze oraz Tarcze\n");
    ImGui::TextWrapped("Kazdy z wymienionych przedmiotow jest klasyfikowany poprzez rzadkosc:\nNormalny, Magiczny, Rzadki, Unikatowy\nod rzadkosci przedmiotu zalezy to ile modyfikatorow statystyk dany przedmiot bedzie posiadal np: Normalny-0, Unikatowy-4");
    ImGui::TextWrapped("Po najechaniu kursorem myszki na przedmiot wyswietla sie jego statystyki, ponizej widoczne sa przykladowe przedmioty losowo wygenerowane");
    ImGui::EndChild();
}

void HelpScene::DrawCreditsTab() {
    ImVec2 childSize = ImVec2(0, ImGui::GetContentRegionAvail().y * 0.7f);
    ImGui::BeginChild("child", childSize, true);
    ImGui::TextWrapped("Created by Dawid Dziegielewski");
    ImGui::TextWrapped("Assets used in this project are from opengameart.org:\n ");
    ImGui::TextWrapped("Audio:\n"
        "File:Death.wav       Author:Michel Baradari  License:CC-BY 3.0\n"
        "File:0.ogg,pain1.wav Author:swuing           License:CC-BY 3.0\n"
        "File:hit.wav         Author:tarfmagougou     License:CC-BY-SA 3.0\n"
        "File:inventory.wav   Author:artisticdude     License:CC-BY 3.0 / CC-BY-SA 3.0 / GPL 2.0 / GPL 3.0\n"
        "File:Sword.wav       Author:remaxim          License:CC0\n"
        "File:test.wav		Author:mrpoly           License:CC0\n\n"
        "Graphics:\n"
        "All Characters Sprites used are from Liberated Pixel Cup\n"
        "https://lpc.opengameart.org/\n"
        "Author:pennomi  License:CC-BY-SA 3.0 / GPL 3.0\n"
        "DungeonTileset       Author:Elthen           License:CC-BY-NC 4.0\n"
        "Gui Elements         Author:Buch             License:CC-BY-SA 3.0\n");
    ImGui::EndChild();
}

const std::string& HelpScene::GetName() const {
    return _name;
}
