#include "ImGuiManager.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_sdl.h"
#include "../GUI/Window.h"
#include <iostream>

// Forward declaration for ImGui types
struct ImGuiViewport;

ImGuiManager& ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return instance;
}

void ImGuiManager::Initialize(SDL_Renderer* renderer, int width, int height) {
    if (_initialized) {
        std::cerr << "ImGuiManager: Already initialized" << std::endl;
        return;
    }
    
    _renderer = renderer;
    
    // Create ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Set .ini file path for window state persistence
    io.IniFilename = _iniFilePath.c_str();
    
    // Initialize ImGuiSDL
    ImGuiSDL::Initialize(renderer, width, height);
    
    // Apply custom style
    ApplyCustomStyle();
    
    _initialized = true;
    std::cout << "ImGuiManager: Initialized successfully" << std::endl;
}

void ImGuiManager::Shutdown() {
    if (!_initialized) return;
    
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    
    _initialized = false;
    std::cout << "ImGuiManager: Shutdown complete" << std::endl;
}

void ImGuiManager::BeginFrame() {
    if (!_initialized) return;
    
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
    if (!_initialized) return;
    
    ImGui::EndFrame();
}

void ImGuiManager::Render() {
    if (!_initialized) return;
    
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
}

void ImGuiManager::ApplyCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Dark theme with game-specific colors
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.95f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.95f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.40f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.15f, 0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.50f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.60f, 0.80f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.50f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.40f, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.35f, 0.45f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.40f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.35f, 0.45f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.50f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.70f, 0.90f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.10f, 0.10f, 0.15f, 0.50f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.50f, 0.70f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.60f, 0.80f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.60f, 0.80f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.30f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.30f, 0.35f);
    
    // Sizing and spacing
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    
    // Borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    
    // Rounding
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
}

void ImGuiManager::LoadStyleFromFile(const std::string& path) {
    // TODO: Implement style loading from file
    // This would load a custom style configuration
}

void ImGuiManager::SaveStyleToFile(const std::string& path) {
    // TODO: Implement style saving to file
    // This would save the current style configuration
}

bool ImGuiManager::BeginCenteredWindow(const char* name, bool* open, ImGuiWindowFlags flags) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImVec2 windowSize = ImVec2(600, 500); // Default size
    
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
    
    return ImGui::Begin(name, open, flags);
}

bool ImGuiManager::BeginModalDialog(const char* name, bool* open, ImVec2 size) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    
    if (size.x == 0 || size.y == 0) {
        size = ImVec2(400, 200); // Default modal size
    }
    
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_Modal | 
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_AlwaysAutoResize;
    
    return ImGui::BeginPopupModal(name, open, flags);
}

void ImGuiManager::SaveSettings() {
    // Settings are automatically saved to .ini file by ImGui
    // This method can be used for custom settings if needed
}

void ImGuiManager::LoadSettings() {
    // Settings are automatically loaded from .ini file by ImGui
    // This method can be used for custom settings if needed
}
