# ImGui Implementation Analysis & Improvements

## Current Implementation Status

### ✅ **What's Working Well**

1. **Proper Initialization**
   - ImGui context created in `Engine::Core_Init()`
   - ImGuiSDL properly initialized with renderer and window dimensions
   - NewFrame/EndFrame/Render cycle correctly implemented in `Core_Render()`

2. **Menu Structure**
   - Options menu with Graphics and Sound tabs
   - Help menu with multiple information tabs
   - Load Game menu with table view
   - Debug UI with performance plots
   - Pause and Death modals

3. **Basic Functionality**
   - Resolution selection (though implementation could be improved)
   - Volume controls with sliders
   - Save file listing and management
   - Tab-based navigation

### ⚠️ **Issues & Areas for Improvement**

#### 1. **No Custom Styling**
- Using default ImGui style (dark theme)
- No visual customization for game's aesthetic
- No style persistence

#### 2. **Settings Don't Persist**
- Volume settings reset on restart
- Resolution changes not saved
- Fullscreen preference not saved
- No settings file integration

#### 3. **Poor Resolution Selection Implementation**
```cpp
// Current: Hacky string concatenation
auto values = ""s;
for (int i = 0; i < SDL_DisplayModes.size(); i++) {
    string test = int_to_str(SDL_DisplayModes[i]->w) + " x " + ...;
    values += test +"\000"s;
}
ImGui::Combo("Resolution", &item_current_2, values.c_str());
```
**Problems:**
- Uses null-terminated string concatenation (fragile)
- No way to identify current resolution
- Doesn't filter duplicate resolutions
- No refresh rate display in combo

#### 4. **Volume Controls Update Immediately**
- Changes applied on every frame (inefficient)
- Should only apply when slider is released or value changes
- No "Apply" button for sound settings

#### 5. **No Input Validation**
- No bounds checking on resolution selection
- No validation of volume values
- Could crash if invalid index selected

#### 6. **Window Management Issues**
- Windows always positioned at 1/4 of screen (not centered)
- No window state persistence (size/position)
- No keyboard shortcuts (ESC to close, etc.)

#### 7. **Memory Management**
- SDL_DisplayMode pointers properly deleted (good!)
- But could use smart pointers for safety

#### 8. **No Settings Integration**
- Settings class exists but not used for ImGui menus
- Should integrate with Settings singleton

#### 9. **Missing Features**
- No keybinding configuration
- No graphics quality settings (VSync, anti-aliasing, etc.)
- No language/localization options
- No accessibility options

#### 10. **Code Organization**
- All ImGui code in Game class (should be in separate scenes)
- No reusable ImGui widget helpers
- Duplicate window positioning code

---

## Recommended Improvements

### **Priority 1: Critical Fixes**

1. **Fix Resolution Selection**
   - Use proper ImGui listbox or combo with array of strings
   - Filter duplicate resolutions
   - Show current resolution
   - Display refresh rate properly

2. **Persist Settings**
   - Save volume, resolution, fullscreen to Settings class
   - Load on startup
   - Apply on game start

3. **Optimize Volume Updates**
   - Only update when slider value actually changes
   - Use ImGui::IsItemDeactivated() to detect slider release

### **Priority 2: User Experience**

4. **Custom Styling**
   - Apply custom ImGui style matching game aesthetic
   - Save/load style preferences
   - Add style editor option

5. **Better Window Management**
   - Center windows properly
   - Add keyboard shortcuts (ESC to close)
   - Remember window positions (using ImGui .ini file)

6. **Input Validation**
   - Validate all user inputs
   - Handle edge cases gracefully

### **Priority 3: Code Quality**

7. **Refactor to Scenes**
   - Move Options menu to OptionsScene
   - Move Help menu to HelpScene
   - Better separation of concerns

8. **Settings Integration**
   - Use Settings singleton for all persistent data
   - Unified settings management

9. **Add More Options**
   - Graphics quality settings
   - Keybinding configuration
   - Language selection

---

## Implementation Plan

### **Step 1: Fix Resolution Selection** (30 min)
- Replace string concatenation with proper array
- Add current resolution detection
- Filter duplicates

### **Step 2: Add Settings Persistence** (1 hour)
- Integrate with Settings class
- Save/load volume, resolution, fullscreen
- Apply on startup

### **Step 3: Optimize Volume Controls** (15 min)
- Track previous values
- Only update when changed

### **Step 4: Custom Styling** (1 hour)
- Create custom ImGui style
- Match game aesthetic
- Add style configuration

### **Step 5: Improve Window Management** (30 min)
- Center windows properly
- Add keyboard shortcuts
- Enable ImGui .ini persistence

### **Step 6: Code Refactoring** (2 hours)
- Create OptionsScene
- Create HelpScene
- Move ImGui code to scenes

---

## Code Examples for Improvements

### **Better Resolution Selection**
```cpp
// Store resolutions in vector
std::vector<std::string> resolutionStrings;
std::vector<SDL_DisplayMode*> validModes;
int currentResolutionIndex = 0;

// Filter and build list
for (int i = 0; i < modes; i++) {
    SDL_DisplayMode* mode = new SDL_DisplayMode{...};
    SDL_GetDisplayMode(display_index, i, mode);
    
    // Check if duplicate
    bool isDuplicate = false;
    for (auto* existing : validModes) {
        if (existing->w == mode->w && existing->h == mode->h) {
            isDuplicate = true;
            break;
        }
    }
    
    if (!isDuplicate) {
        validModes.push_back(mode);
        std::string resStr = std::to_string(mode->w) + " x " + 
                           std::to_string(mode->h) + " @ " + 
                           std::to_string(mode->refresh_rate) + "Hz";
        resolutionStrings.push_back(resStr);
        
        // Check if current resolution
        if (mode->w == Window::GetWidth() && 
            mode->h == Window::GetHeight()) {
            currentResolutionIndex = resolutionStrings.size() - 1;
        }
    }
}

// Use in ImGui
const char* combo_preview = resolutionStrings[currentResolutionIndex].c_str();
if (ImGui::BeginCombo("Resolution", combo_preview)) {
    for (int i = 0; i < resolutionStrings.size(); i++) {
        bool is_selected = (currentResolutionIndex == i);
        if (ImGui::Selectable(resolutionStrings[i].c_str(), is_selected)) {
            currentResolutionIndex = i;
        }
        if (is_selected) {
            ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndCombo();
}
```

### **Settings Persistence**
```cpp
// In Settings class
void Settings::SaveImGuiSettings() {
    // Save to JSON or config file
    SetInt("MasterVolume", Audio::GetMasterVolume());
    SetInt("MusicVolume", Audio::GetMusicVolume());
    SetBool("Fullscreen", _engine->check_fullscreen);
    SetInt("ResolutionWidth", Window::GetWidth());
    SetInt("ResolutionHeight", Window::GetHeight());
}

void Settings::LoadImGuiSettings() {
    // Load from file
    Audio::SetMasterVolume(GetInt("MasterVolume", 50));
    Audio::SetMusicVolume(GetInt("MusicVolume", 50));
    _engine->check_fullscreen = GetBool("Fullscreen", false);
    // Apply resolution on next frame
}
```

### **Custom Styling**
```cpp
void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Customize colors
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.15f, 0.95f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.7f, 1.0f);
    
    // Customize sizes
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    
    // Enable ImGui .ini file for window state persistence
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "imgui.ini";
}
```

---

**Status**: Analysis Complete - Ready for Implementation
