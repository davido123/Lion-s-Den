# ImGui Integration with Game Engine - Implementation Plan

## 📊 Current State Analysis

### ✅ **What's Working**
1. **Basic Integration**
   - ImGui initialized in `Engine::Core_Init()`
   - NewFrame/EndFrame/Render cycle in `Engine::Core_Render()`
   - ImGuiSDL rendering backend functional
   - Custom styling applied

2. **Menu Functionality**
   - Options menu (Graphics, Sound tabs)
   - Help menu (multiple tabs)
   - Load Game menu (table view)
   - Debug UI (performance plots)
   - Pause/Death modals

3. **Recent Fixes**
   - Window state tracking
   - ESC key handling
   - Mouse capture management
   - Scene state synchronization

### ⚠️ **Architectural Issues**

#### 1. **Tight Coupling with Game Class**
```cpp
// Current: All ImGui menus in Game class
Engine::Core_Render() {
    OnRenderDebugGui();    // Game::OnRenderDebugGui()
    DrawOptionsMenu();     // Game::DrawOptionsMenu()
    DrawLoadMenu();        // Game::DrawLoadMenu()
    DrawHelpMenu();        // Game::DrawHelpMenu()
}
```
**Problems:**
- ImGui code not scene-aware
- Can't have scene-specific ImGui UI
- Hard to add new scenes with ImGui
- Violates separation of concerns

#### 2. **No Scene-Based ImGui Rendering**
- Scenes can't render their own ImGui UI
- All ImGui calls must go through Game class
- No way to have different ImGui layouts per scene

#### 3. **State Management Split**
- `ShowOptions`, `ShowLoad`, `ShowHelp` flags in Game class
- Scene state (`_showOptions`, etc.) in MainMenuScene
- Requires manual synchronization
- Error-prone and fragile

#### 4. **No ImGui Manager/Helper System**
- Duplicate window positioning code
- No reusable ImGui components
- No centralized ImGui configuration
- Hard to maintain consistent UI

---

## 🎯 **Integration Goals**

1. **Scene-Based ImGui Rendering**
   - Each scene can render its own ImGui UI
   - Scenes control their own menu visibility
   - No dependency on Game class for ImGui

2. **Unified State Management**
   - Single source of truth for UI state
   - Automatic synchronization
   - Type-safe state management

3. **Reusable ImGui Components**
   - Common window helpers
   - Standardized menu layouts
   - Shared styling utilities

4. **Better Architecture**
   - Clear separation of concerns
   - Easy to extend
   - Maintainable code structure

---

## 📋 **Implementation Plan**

### **Day 4: ImGui Architecture Refactoring**

#### **Phase 1: Create ImGui Manager System** (2-3 hours)

**Task 1.1: Create ImGuiManager Class**
- **File**: `Systems/ImGuiManager.h` and `Systems/ImGuiManager.cpp`
- **Purpose**: Centralized ImGui management
- **Features**:
  ```cpp
  class ImGuiManager {
  public:
      static ImGuiManager& GetInstance();
      
      // Initialization
      void Initialize(SDL_Renderer* renderer, int width, int height);
      void Shutdown();
      
      // Frame lifecycle
      void BeginFrame();
      void EndFrame();
      void Render();
      
      // Style management
      void ApplyCustomStyle();
      void LoadStyleFromFile(const std::string& path);
      void SaveStyleToFile(const std::string& path);
      
      // Window helpers
      bool BeginCenteredWindow(const char* name, bool* open, ImGuiWindowFlags flags);
      bool BeginModalDialog(const char* name, bool* open, ImVec2 size);
      
      // Settings persistence
      void SaveSettings();
      void LoadSettings();
      
  private:
      ImGuiManager() = default;
      ~ImGuiManager() = default;
      ImGuiManager(const ImGuiManager&) = delete;
      ImGuiManager& operator=(const ImGuiManager&) = delete;
      
      bool _initialized = false;
      std::string _iniFilePath = "imgui.ini";
  };
  ```

**Task 1.2: Create ImGuiHelper Utilities**
- **File**: `GUI/ImGuiHelper.h` and `GUI/ImGuiHelper.cpp`
- **Purpose**: Reusable ImGui components
- **Features**:
  ```cpp
  namespace ImGuiHelper {
      // Window positioning
      void CenterWindow(ImVec2 size);
      void CenterWindowOnScreen(ImVec2 size);
      
      // Common UI patterns
      bool ButtonCentered(const char* label, ImVec2 size = ImVec2(0, 0));
      void SeparatorCentered();
      void TextCentered(const char* text);
      
      // Menu helpers
      bool MenuButton(const char* label, ImVec2 size);
      void MenuHeader(const char* title);
      
      // Settings widgets
      bool ResolutionCombo(const char* label, int* currentIndex, 
                          std::vector<SDL_DisplayMode*>& modes);
      bool VolumeSlider(const char* label, int* volume, int min, int max);
  }
  ```

**Task 1.3: Integrate ImGuiManager into Engine**
- **Location**: `Engine.cpp`
- **Changes**:
  ```cpp
  void Engine::Core_Init() {
      // ... existing code ...
      ImGuiManager::GetInstance().Initialize(
          Window::GetRenderer(), 
          Window::GetWidth(), 
          Window::GetHeight()
      );
  }
  
  void Engine::Core_Render() {
      ImGuiManager::GetInstance().BeginFrame();
      
      // ... existing rendering ...
      
      // Let scenes render their ImGui UI
      SceneManager::GetInstance().RenderImGui();
      
      // Global ImGui UI (debug, etc.)
      OnRenderDebugGui();
      
      ImGuiManager::GetInstance().EndFrame();
      ImGuiManager::GetInstance().Render();
  }
  ```

---

#### **Phase 2: Scene-Based ImGui Rendering** (3-4 hours)

**Task 2.1: Add ImGui Rendering to Scene Base Class**
- **File**: `Scenes/Scene.h`
- **Changes**:
  ```cpp
  class Scene {
  public:
      // ... existing methods ...
      
      // ImGui rendering (optional, scenes can override)
      virtual void RenderImGui() {}
      
  protected:
      Engine* _engine = nullptr;
  };
  ```

**Task 2.2: Create OptionsScene**
- **Files**: `Scenes/OptionsScene.h` and `Scenes/OptionsScene.cpp`
- **Purpose**: Dedicated scene for options menu
- **Features**:
  ```cpp
  class OptionsScene : public Scene {
  public:
      OptionsScene();
      ~OptionsScene() override = default;
      
      void Enter() override;
      void Exit() override;
      void HandleEvent(SDL_Event* event, const Uint8* keyboardState) override;
      void Update() override;
      void Render() override;
      void RenderImGui() override;  // NEW: Render options menu
      const std::string& GetName() const override;
      
      // State management
      void Show(bool show) { _visible = show; }
      bool IsVisible() const { return _visible; }
      
  private:
      bool _visible = false;
      void DrawGraphicsTab();
      void DrawSoundTab();
      void DrawControlsTab();
  };
  ```

**Task 2.3: Create HelpScene**
- **Files**: `Scenes/HelpScene.h` and `Scenes/HelpScene.cpp`
- **Purpose**: Dedicated scene for help menu
- **Features**:
  ```cpp
  class HelpScene : public Scene {
  public:
      // Similar structure to OptionsScene
      void RenderImGui() override;  // Render help menu
      
  private:
      bool _visible = false;
      void DrawIntroTab();
      void DrawControlsTab();
      void DrawMechanicsTab();
      void DrawCreditsTab();
  };
  ```

**Task 2.4: Create LoadGameScene**
- **Files**: `Scenes/LoadGameScene.h` and `Scenes/LoadGameScene.cpp`
- **Purpose**: Dedicated scene for load game menu
- **Features**:
  ```cpp
  class LoadGameScene : public Scene {
  public:
      // Similar structure
      void RenderImGui() override;  // Render load game menu
      
  private:
      bool _visible = false;
      void DrawSaveFileTable();
      void DrawNewGameDialog();
  };
  ```

**Task 2.5: Update SceneManager for ImGui Rendering**
- **File**: `Systems/SceneManager.cpp`
- **Changes**:
  ```cpp
  void SceneManager::RenderImGui() {
      if (_currentScene) {
          _currentScene->RenderImGui();
      }
  }
  ```

**Task 2.6: Update MainMenuScene**
- **File**: `Scenes/MainMenuScene.cpp`
- **Changes**:
  - Remove ImGui menu rendering (moved to dedicated scenes)
  - Update button handlers to switch to OptionsScene/HelpScene/LoadGameScene
  - Remove `_showOptions`, `_showLoad`, `_showHelp` flags

**Task 2.7: Update GameScene**
- **File**: `Scenes/GameScene.cpp`
- **Changes**:
  - Move pause menu ImGui rendering to `RenderImGui()`
  - Move death dialog to `RenderImGui()`
  - Keep gameplay rendering in `Render()`

---

#### **Phase 3: State Management Refactoring** (2-3 hours)

**Task 3.1: Create UIManager for State Management**
- **File**: `Systems/UIManager.h` and `Systems/UIManager.cpp`
- **Purpose**: Centralized UI state management
- **Features**:
  ```cpp
  class UIManager {
  public:
      static UIManager& GetInstance();
      
      // Menu visibility
      void ShowOptions(bool show);
      bool IsOptionsVisible() const { return _showOptions; }
      
      void ShowHelp(bool show);
      bool IsHelpVisible() const { return _showHelp; }
      
      void ShowLoadGame(bool show);
      bool IsLoadGameVisible() const { return _showLoadGame; }
      
      // Event handling
      void HandleEvent(SDL_Event* event);
      
      // Scene switching helpers
      void OpenOptions();
      void OpenHelp();
      void OpenLoadGame();
      void CloseAllMenus();
      
  private:
      bool _showOptions = false;
      bool _showHelp = false;
      bool _showLoadGame = false;
      
      SceneManager* _sceneManager = nullptr;
  };
  ```

**Task 3.2: Integrate UIManager with Scenes**
- Update MainMenuScene to use UIManager
- Update OptionsScene/HelpScene/LoadGameScene to register with UIManager
- Remove state flags from Game class

---

#### **Phase 4: Remove Game Class Dependency** (2-3 hours)

**Task 4.1: Remove ImGui Methods from Game Class**
- **File**: `Game/Game.h` and `Game/Game.cpp`
- **Remove**:
  - `DrawOptionsMenu()`
  - `DrawLoadMenu()`
  - `DrawHelpMenu()`
  - `ShowOptions`, `ShowLoad`, `ShowHelp` flags
  - `LoadImGuiSettings()`, `SaveImGuiSettings()`

**Task 4.2: Update Engine Virtual Methods**
- **File**: `Engine.h` and `main.cpp`
- **Remove**:
  - `virtual void DrawOptionsMenu()`
  - `virtual void DrawLoadMenu()`
  - `virtual void DrawHelpMenu()`
- **Keep**:
  - `OnRenderDebugGui()` (for debug UI)

**Task 4.3: Update Engine::Core_Render()**
- **File**: `Engine.cpp`
- **Changes**:
  ```cpp
  void Engine::Core_Render() {
      ImGuiManager::GetInstance().BeginFrame();
      
      // ... existing rendering ...
      
      // Scene-based ImGui rendering
      SceneManager::GetInstance().RenderImGui();
      
      // Global debug UI
      OnRenderDebugGui();
      
      ImGuiManager::GetInstance().EndFrame();
      ImGuiManager::GetInstance().Render();
  }
  ```

---

### **Day 5: Advanced Features & Polish** (Optional)

#### **Phase 5: Advanced ImGui Features** (3-4 hours)

**Task 5.1: Settings Persistence System**
- Integrate with Settings class
- Save/load all ImGui settings
- Auto-apply on startup

**Task 5.2: ImGui Theme System**
- Multiple theme presets
- Theme editor
- Save/load themes

**Task 5.3: ImGui Widget Library**
- Reusable game-specific widgets
- Inventory display widget
- Stats display widget
- Item tooltip widget

**Task 5.4: Performance Optimization**
- ImGui draw call batching
- Conditional rendering
- LOD for complex widgets

---

## 📐 **New Architecture**

### **Before (Current)**
```
Engine::Core_Render()
  ├── ImGui::NewFrame()
  ├── SceneManager::Render()        (game rendering)
  ├── Game::OnRenderDebugGui()       (debug UI)
  ├── Game::DrawOptionsMenu()        (options - Game class)
  ├── Game::DrawLoadMenu()           (load - Game class)
  ├── Game::DrawHelpMenu()           (help - Game class)
  ├── ImGui::EndFrame()
  └── ImGui::Render()
```

### **After (Proposed)**
```
Engine::Core_Render()
  ├── ImGuiManager::BeginFrame()
  ├── SceneManager::Render()         (game rendering)
  ├── SceneManager::RenderImGui()    (scene ImGui UI)
  │   ├── MainMenuScene::RenderImGui()
  │   ├── OptionsScene::RenderImGui()    (if visible)
  │   ├── HelpScene::RenderImGui()       (if visible)
  │   └── LoadGameScene::RenderImGui()  (if visible)
  ├── GameScene::RenderImGui()       (pause menu, death dialog)
  ├── OnRenderDebugGui()             (global debug UI)
  ├── ImGuiManager::EndFrame()
  └── ImGuiManager::Render()
```

---

## 🎯 **Benefits**

1. **Better Architecture**
   - Clear separation of concerns
   - Scene-specific ImGui UI
   - No Game class dependency

2. **Easier Maintenance**
   - Reusable components
   - Centralized management
   - Consistent patterns

3. **Better Extensibility**
   - Easy to add new scenes with ImGui
   - Easy to add new menu types
   - Easy to customize per scene

4. **Type Safety**
   - Centralized state management
   - No manual synchronization
   - Compile-time checks

5. **Performance**
   - Conditional rendering
   - Optimized draw calls
   - Better resource management

---

## ⚠️ **Migration Strategy**

### **Step 1: Add New System (Non-Breaking)**
- Create ImGuiManager, UIManager
- Create new scenes (OptionsScene, HelpScene, LoadGameScene)
- Keep existing Game class methods working

### **Step 2: Gradual Migration**
- Update MainMenuScene to use new scenes
- Move ImGui rendering to scenes
- Test each scene independently

### **Step 3: Remove Old Code**
- Remove ImGui methods from Game class
- Remove state flags from Game class
- Clean up Engine virtual methods

### **Step 4: Polish**
- Add advanced features
- Optimize performance
- Add documentation

---

## 📝 **Implementation Checklist**

### **Day 4 Tasks**
- [ ] Create ImGuiManager class
- [ ] Create ImGuiHelper utilities
- [ ] Integrate ImGuiManager into Engine
- [ ] Add RenderImGui() to Scene base class
- [ ] Create OptionsScene
- [ ] Create HelpScene
- [ ] Create LoadGameScene
- [ ] Update SceneManager for ImGui rendering
- [ ] Update MainMenuScene to use new scenes
- [ ] Update GameScene to use RenderImGui()
- [ ] Create UIManager for state management
- [ ] Remove ImGui methods from Game class
- [ ] Update Engine::Core_Render()
- [ ] Test all menu functionality

### **Day 5 Tasks (Optional)**
- [ ] Settings persistence system
- [ ] ImGui theme system
- [ ] Widget library
- [ ] Performance optimization
- [ ] Documentation

---

## 🔧 **Code Examples**

### **Example: OptionsScene Implementation**
```cpp
void OptionsScene::RenderImGui() {
    if (!_visible) return;
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImVec2 size(600, 500);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
    
    bool windowOpen = true;
    if (ImGui::Begin("Options", &windowOpen, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        
        if (ImGui::BeginTabBar("OptionsTabs")) {
            if (ImGui::BeginTabItem("Graphics")) {
                DrawGraphicsTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Sound")) {
                DrawSoundTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Controls")) {
                DrawControlsTab();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            windowOpen = false;
        }
    }
    ImGui::End();
    
    if (!windowOpen) {
        UIManager::GetInstance().ShowOptions(false);
    }
}
```

### **Example: UIManager Integration**
```cpp
void UIManager::OpenOptions() {
    _showOptions = true;
    SceneManager& sm = SceneManager::GetInstance();
    OptionsScene* optionsScene = dynamic_cast<OptionsScene*>(sm.GetScene("Options"));
    if (optionsScene) {
        optionsScene->Show(true);
    }
}

void MainMenuScene::HandleButtonClicks() {
    if (clickedLabel == "Options") {
        UIManager::GetInstance().OpenOptions();
    }
    // ... other buttons
}
```

---

## 📊 **Estimated Time**

- **Day 4 (Core Refactoring)**: 8-10 hours
- **Day 5 (Advanced Features)**: 3-4 hours (optional)
- **Total**: 11-14 hours

---

## ✅ **Success Criteria**

1. All ImGui menus work in scene-based system
2. No Game class dependency for ImGui
3. Scenes can render their own ImGui UI
4. State management is centralized
5. Code is maintainable and extensible
6. No regressions in functionality
7. Performance is maintained or improved

---

**Status**: 📋 Plan Ready for Implementation

**Next Step**: Begin Day 4 - Phase 1: Create ImGuiManager System
