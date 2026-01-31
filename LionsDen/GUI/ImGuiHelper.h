#pragma once

#include "../ImGui/imgui.h"
#include <string>
#include <vector>
#include <SDL.h>

/**
 * ImGuiHelper - Reusable ImGui UI components and utilities
 * Provides common patterns and widgets for consistent UI across scenes
 */
namespace ImGuiHelper {
    // Window positioning
    void CenterWindow(ImVec2 size);
    void CenterWindowOnScreen(ImVec2 size);
    void CenterWindowAtPosition(ImVec2 pos, ImVec2 size);
    
    // Common UI patterns
    bool ButtonCentered(const char* label, ImVec2 size = ImVec2(0, 0));
    void SeparatorCentered();
    void TextCentered(const char* text);
    void TextWrappedCentered(const char* text);
    
    // Menu helpers
    bool MenuButton(const char* label, ImVec2 size = ImVec2(120, 0));
    void MenuHeader(const char* title);
    void MenuFooter();
    
    // Settings widgets
    bool ResolutionCombo(const char* label, int* currentIndex, 
                        std::vector<SDL_DisplayMode*>& modes,
                        std::vector<std::string>& modeStrings);
    bool VolumeSlider(const char* label, int* volume, int min = 0, int max = 100);
    bool CheckboxWithLabel(const char* label, bool* value);
    
    // Dialog helpers
    bool BeginCenteredModal(const char* name, bool* open, ImVec2 size = ImVec2(400, 200));
    void EndCenteredModal();
    
    // Layout helpers
    void Spacing(int count = 1);
    void SameLineWithSpacing(float spacing = 8.0f);
}
