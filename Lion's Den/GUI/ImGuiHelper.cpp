#include "ImGuiHelper.h"
#include "../ImGui/imgui.h"
#include "../GUI/Window.h"
#include <algorithm>

void ImGuiHelper::CenterWindow(ImVec2 size) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
}

void ImGuiHelper::CenterWindowOnScreen(ImVec2 size) {
    CenterWindow(size);
}

void ImGuiHelper::CenterWindowAtPosition(ImVec2 pos, ImVec2 size) {
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
}

bool ImGuiHelper::ButtonCentered(const char* label, ImVec2 size) {
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float buttonWidth = size.x > 0 ? size.x : ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    
    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    return ImGui::Button(label, size);
}

void ImGuiHelper::SeparatorCentered() {
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float separatorWidth = windowWidth * 0.8f;
    float startPos = (windowWidth - separatorWidth) * 0.5f;
    
    ImGui::SetCursorPosX(startPos);
    ImGui::Separator();
}

void ImGuiHelper::TextCentered(const char* text) {
    float windowWidth = ImGui::GetContentRegionAvail().x;
    float textWidth = ImGui::CalcTextSize(text).x;
    
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text);
}

void ImGuiHelper::TextWrappedCentered(const char* text) {
    float windowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::PushTextWrapPos(windowWidth);
    
    // Calculate text height for centering
    ImVec2 textSize = ImGui::CalcTextSize(text, nullptr, false, windowWidth);
    float startY = ImGui::GetCursorPosY();
    
    ImGui::TextWrapped("%s", text);
    
    // Center vertically if needed
    float endY = ImGui::GetCursorPosY();
    float textHeight = endY - startY;
    // Note: Vertical centering is more complex and may not always be desired
    // This is a basic implementation
    
    ImGui::PopTextWrapPos();
}

bool ImGuiHelper::MenuButton(const char* label, ImVec2 size) {
    if (size.x == 0) {
        size.x = 120.0f;
    }
    if (size.y == 0) {
        size.y = ImGui::GetFrameHeight();
    }
    return ImGui::Button(label, size);
}

void ImGuiHelper::MenuHeader(const char* title) {
    ImGui::Separator();
    TextCentered(title);
    ImGui::Separator();
    ImGui::Spacing();
}

void ImGuiHelper::MenuFooter() {
    ImGui::Spacing();
    ImGui::Separator();
}

bool ImGuiHelper::ResolutionCombo(const char* label, int* currentIndex, 
                                  std::vector<SDL_DisplayMode*>& modes,
                                  std::vector<std::string>& modeStrings) {
    if (modeStrings.empty() || *currentIndex < 0 || *currentIndex >= modeStrings.size()) {
        return false;
    }
    
    const char* preview = modeStrings[*currentIndex].c_str();
    if (ImGui::BeginCombo(label, preview)) {
        for (int i = 0; i < modeStrings.size(); i++) {
            bool isSelected = (*currentIndex == i);
            if (ImGui::Selectable(modeStrings[i].c_str(), isSelected)) {
                *currentIndex = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
        return true;
    }
    return false;
}

bool ImGuiHelper::VolumeSlider(const char* label, int* volume, int min, int max) {
    return ImGui::SliderInt(label, volume, min, max);
}

bool ImGuiHelper::CheckboxWithLabel(const char* label, bool* value) {
    return ImGui::Checkbox(label, value);
}

bool ImGuiHelper::BeginCenteredModal(const char* name, bool* open, ImVec2 size) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    
    if (size.x == 0 || size.y == 0) {
        size = ImVec2(400, 200);
    }
    
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_Modal | 
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_AlwaysAutoResize;
    
    return ImGui::BeginPopupModal(name, open, flags);
}

void ImGuiHelper::EndCenteredModal() {
    ImGui::EndPopup();
}

void ImGuiHelper::Spacing(int count) {
    for (int i = 0; i < count; i++) {
        ImGui::Spacing();
    }
}

void ImGuiHelper::SameLineWithSpacing(float spacing) {
    ImGui::SameLine(0, spacing);
}
