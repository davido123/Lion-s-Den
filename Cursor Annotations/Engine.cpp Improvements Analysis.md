# Engine.cpp Improvements Analysis

## 🔍 **Issues Identified**

### **1. Code Duplication (Critical)**
- **Config file reading duplicated** (lines 377-408 and 410-441)
  - Same code reads `config.txt` twice
  - Duplicate variable declarations (`w`, `h`)
  
- **Duplicate function calls**:
  - `OnInit()` called twice (lines 480, 482)
  - `OnEvent()` called twice (lines 531, 534)
  - `OnRenderDebugGui()` called twice (lines 664, 667)
  - `Window::SetMode()` called twice (lines 444-445)
  - `Collider::ProcessAttackCollisions()` called twice (lines 621, 628)

- **Duplicate comments**: Many comments appear twice throughout the file (e.g., lines 120-128, 133-134, etc.)

- **Duplicate variable declarations**:
  - `mouseX`, `mouseY` declared twice (lines 565-566, 568-569)
  - `ImGuiIO& io` declared twice (lines 574, 580)
  - Layer update loop duplicated (lines 605-611)

- **Duplicate comments**: Many comments appear twice throughout the file

### **2. Unused Code**
- **ExampleAppLog struct** (lines 10-111): Defined but never used
- **_DeleteCandidates vector**: Declared in Engine.h but never used (uses `Object::DeleteCandidates` instead)

### **3. Performance Issues**
- **Inefficient frame time tracking**: Using `erase(begin())` on vector (O(n) operation)
  - Should use `std::deque` or circular buffer for O(1) operations
  - Lines 210-212, 239-241, 627

- **Memory allocation**: `new SDL_Event` (line 146)
  - Should use stack allocation: `SDL_Event event;`
  - Or smart pointer if needed

- **Hardcoded DeltaTime**: `io.DeltaTime = 1.0f / 60.0f;` (lines 575, 581)
  - Should calculate from actual frame time

### **4. Code Quality Issues**
- **Inconsistent error handling**: Missing null checks
- **Magic numbers**: Hardcoded values (100, 60, 1000)
- **Poor variable naming**: `w`, `h` instead of `width`, `height`
- **Inconsistent formatting**: Mixed spacing and indentation
- **Function scope issue**: `SetVideo()` is not a member function (line 129) - should be `Engine::SetVideo`
- **Bug in SDL_SetRenderDrawColor**: Wrong parameter order (line 644) - `bk.r, bk.b, bk.g` should be `bk.r, bk.g, bk.b`

### **5. Logic Issues**
- **Frame time tracking location**: Frame times tracked in main loop instead of where work is done
- **Render timing**: Render time tracked but includes ImGui rendering
- **Update timing**: Logic time includes collision processing

---

## ✅ **Recommended Improvements**

### **Improvement 0: Fix Critical Bugs** (Priority: Critical)

1. **Fix SetVideo Function Scope**:
```cpp
// BEFORE (line 129):
void SetVideo(int w, int h, bool full_screen, std::string win_title) {
    Window::SetMode(w, h, full_screen, win_title);
}

// AFTER:
void Engine::SetVideo(int w, int h, bool full_screen, std::string win_title) {
    Window::SetMode(w, h, full_screen, win_title);
}
```

2. **Fix SDL_SetRenderDrawColor Bug**:
```cpp
// BEFORE (line 644):
SDL_SetRenderDrawColor(Window::GetRenderer(), bk.r, bk.b, bk.g, bk.a);
// Wrong order: r, b, g instead of r, g, b

// AFTER:
SDL_SetRenderDrawColor(Window::GetRenderer(), bk.r, bk.g, bk.b, bk.a);
// Correct order: r, g, b
```

### **Improvement 1: Remove Code Duplication** (Priority: High)

**File: `Lion's Den/Engine.cpp`**

1. **Remove duplicate config reading**:
```cpp
// BEFORE (lines 377-441):
std::ifstream cFile ("config.txt");
// ... reading code ...
std::ifstream cFile ("config.txt");  // DUPLICATE
// ... same reading code ...

// AFTER:
int w = 1280, h = 1024; // Default values
std::ifstream cFile("config.txt");
if (cFile.is_open()) {
    std::string line;
    while (getline(cFile, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.empty() || line[0] == '#') continue;
        
        auto delimiterPos = line.find("=");
        if (delimiterPos == std::string::npos) continue;
        
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);
        
        if (name == "w") w = std::stoi(value);
        else if (name == "h") h = std::stoi(value);
    }
    cFile.close();
}
```

2. **Remove duplicate function calls**:
```cpp
// BEFORE:
OnInit(); 
OnInit();  // DUPLICATE

// AFTER:
OnInit();
```

3. **Remove duplicate variable declarations**:
```cpp
// BEFORE:
int mouseX, mouseY;
const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
int mouseX, mouseY;  // DUPLICATE
const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

// AFTER:
int mouseX, mouseY;
const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
```

### **Improvement 2: Optimize Frame Time Tracking** (Priority: Medium)

**Replace vector with deque for O(1) operations**:

```cpp
// BEFORE:
std::vector<float>& RenderFrameTimes = DebugUI::GetInstance("1")->getRenderFrameTimes();
RenderFrameTimes.push_back(RenderTicksElapsed);
if (RenderFrameTimes.size() > 100) {
    RenderFrameTimes.erase(RenderFrameTimes.begin()); // O(n)
}

// AFTER:
std::deque<float>& RenderFrameTimes = DebugUI::GetInstance("1")->getRenderFrameTimes();
RenderFrameTimes.push_back(RenderTicksElapsed);
if (RenderFrameTimes.size() > 100) {
    RenderFrameTimes.pop_front(); // O(1)
}
```

**Note**: This requires updating `DebugUI.h` to use `std::deque` instead of `std::vector`.

### **Improvement 3: Use Stack Allocation for SDL_Event** (Priority: Low)

```cpp
// BEFORE:
SDL_Event* event = new SDL_Event;
// ... use event ...
delete (event);

// AFTER:
SDL_Event event;
// ... use &event ...
// No delete needed
```

### **Improvement 4: Calculate DeltaTime from Actual Frame Time** (Priority: Medium)

```cpp
// BEFORE:
io.DeltaTime = 1.0f / 60.0f; // Hardcoded

// AFTER:
static Uint32 lastFrameTime = SDL_GetTicks();
Uint32 currentFrameTime = SDL_GetTicks();
io.DeltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
if (io.DeltaTime <= 0.0f) io.DeltaTime = 1.0f / 60.0f; // Fallback
lastFrameTime = currentFrameTime;
```

### **Improvement 5: Remove Unused Code** (Priority: Low)

1. **Remove ExampleAppLog struct** (lines 10-111)
2. **Remove _DeleteCandidates** from Engine.h (line 61) - never used

### **Improvement 6: Extract Config Reading to Helper Function** (Priority: Medium)

```cpp
// Add to Engine.cpp:
bool Engine::LoadConfig(int& width, int& height) {
    width = 1280;  // Default
    height = 1024; // Default
    
    std::ifstream cFile("config.txt");
    if (!cFile.is_open()) {
        std::cerr << "Couldn't open config file, using defaults.\n";
        return false;
    }
    
    std::string line;
    while (getline(cFile, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.empty() || line[0] == '#') continue;
        
        auto delimiterPos = line.find("=");
        if (delimiterPos == std::string::npos) continue;
        
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);
        
        if (name == "w") width = std::stoi(value);
        else if (name == "h") height = std::stoi(value);
    }
    
    cFile.close();
    return true;
}
```

### **Improvement 7: Clean Up Main Loop** (Priority: Medium)

**Extract frame time tracking to helper methods**:

```cpp
// Add to Engine class:
void Engine::TrackRenderTime(Uint32 elapsed) {
    auto& times = DebugUI::GetInstance("1")->getRenderFrameTimes();
    times.push_back(static_cast<float>(elapsed));
    if (times.size() > 100) {
        times.pop_front(); // If using deque
    }
}

void Engine::TrackLogicTime(Uint32 elapsed) {
    auto& times = DebugUI::GetInstance("1")->getLogicFrameTimes();
    times.push_back(static_cast<float>(elapsed));
    if (times.size() > 100) {
        times.pop_front();
    }
}
```

### **Improvement 8: Fix Layer Update Loop** (Priority: Low)

```cpp
// BEFORE:
int length = _Layers.size();
for(int i = 0; i < length; ++i){
    // Empty
}
for(int i = 0; i < _Layers.size(); ++i){
    _Layers[i]->UpdateChildren();
}

// AFTER:
for (auto* layer : _Layers) {
    if (layer) {
        layer->UpdateChildren();
    }
}
```

### **Improvement 9: Remove Duplicate Collision Processing** (Priority: High)

```cpp
// BEFORE:
Collider::ProcessAttackCollisions();
// ... other code ...
Collider::ProcessAttackCollisions(); // DUPLICATE

// AFTER:
Collider::ProcessAttackCollisions(); // Once
```

---

## 📋 **Implementation Priority**

### **Phase 1: Critical Fixes (Do First)**
1. ✅ Remove duplicate function calls (OnInit, OnEvent, OnRenderDebugGui)
2. ✅ Remove duplicate config file reading
3. ✅ Remove duplicate collision processing
4. ✅ Fix duplicate variable declarations

### **Phase 2: Code Quality (Do Second)**
5. ✅ Extract config reading to helper function
6. ✅ Remove unused ExampleAppLog struct
7. ✅ Clean up layer update loop
8. ✅ Remove duplicate comments

### **Phase 3: Performance (Do Third)**
9. ✅ Optimize frame time tracking (vector → deque)
10. ✅ Use stack allocation for SDL_Event
11. ✅ Calculate DeltaTime from actual frame time

### **Phase 4: Cleanup (Do Last)**
12. ✅ Remove unused _DeleteCandidates
13. ✅ Add error handling
14. ✅ Improve variable naming

---

## 🔧 **Quick Fixes (Can Do Now)**

### **Fix 1: Remove Duplicate Calls**
```cpp
// In Core_Init() - line 480-482:
OnInit(); // Remove duplicate

// In Core_Event() - line 531-534:
OnEvent(event, keyboardState); // Remove duplicate

// In Core_Render() - line 664-667:
OnRenderDebugGui(); // Remove duplicate
```

### **Fix 2: Remove Duplicate Config Reading**
```cpp
// In Core_Init() - remove lines 410-441 (duplicate of 377-408)
```

### **Fix 3: Fix Duplicate Variable Declarations**
```cpp
// In Core_Update() - remove duplicate declarations (lines 568-585)
// Keep only one set
```

### **Fix 4: Fix Duplicate Collision Processing**
```cpp
// In Core_Update() - remove line 628 (duplicate of 621)
```

---

## 📊 **Impact Assessment**

### **Before Improvements**
- **Lines of Code**: ~713
- **Duplicate Code**: ~150 lines
- **Performance Issues**: O(n) operations in hot path
- **Maintainability**: Low (duplicates cause confusion)

### **After Improvements**
- **Lines of Code**: ~560 (20% reduction)
- **Duplicate Code**: 0 lines
- **Performance**: O(1) operations
- **Maintainability**: High (clean, organized)

---

## 🎯 **Integration with Implementation Plan**

These improvements should be added to:

### **Day 1 (Additional Task)**
- **Task 1.4: Clean Up Engine.cpp** (1-2 hours)
  - Remove all duplicate code
  - Remove unused ExampleAppLog
  - Fix duplicate function calls
  - Extract config reading

### **Day 5 (Additional Task)**
- **Task 5.6: Optimize Engine Performance** (1-2 hours)
  - Replace vector with deque for frame times
  - Use stack allocation for SDL_Event
  - Calculate DeltaTime from frame time
  - Clean up main loop

---

*These improvements will make the engine more maintainable, faster, and easier to work with.*
