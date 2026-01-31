# ImGui System Implementation — Analysis & Problems

Analysis of the current ImGui integration (Engine, ImGuiManager, scenes, UIManager) and identified issues.

---

## 1. Critical: Double ImGui context creation and leak

**Where:** `Engine::Core_Init()` (lines 173–222) and `ImGuiManager::Initialize()` (line 24).

**What:**  
- `Core_Init()` calls `ImGui::CreateContext()`, sets `io.IniFilename` and a full custom style.  
- Then it calls `ImGuiManager::GetInstance().Initialize(...)`, which calls `ImGui::CreateContext()` again.  
- The second `CreateContext()` becomes the current context; the first is never destroyed.  
- `ImGuiManager::Shutdown()` only destroys the current (second) context.  
- Result: one context leak per run, and the style/ini set in `Core_Init()` apply to the leaked context, not the one actually used.

**Fix:**  
- Remove from `Core_Init()`: the entire block that calls `ImGui::CreateContext()`, sets `io.IniFilename`, and applies the custom style (lines 173–222).  
- Keep a single init path: only `ImGuiManager::Initialize()` creates the context, sets ini path, and applies style (already does the latter via `ApplyCustomStyle()`).

---

## 2. UIManager::HandleEvent never called

**Where:** `UIManager` has `HandleEvent(void* event)` to close Options/Help/LoadGame on ESC, but nothing in the engine or main loop calls it.

**What:**  
- ESC to close menus is only handled inside each scene’s `RenderImGui()` (e.g. `OptionsScene`: `ImGui::IsKeyPressed(ImGuiKey_Escape)`).  
- If ESC is consumed elsewhere, or event order changes, or ImGui doesn’t have focus, menus may not close.  
- There is no single place that guarantees “ESC closes any open overlay menu”.

**Fix:**  
- In `Engine::Core_Event()`, after `ImGui_ImplSDL2_ProcessEvent(event)` (and optionally after `SceneManager::HandleEvent`), call:  
  `UIManager::GetInstance().HandleEvent(event);`  
- Ensure `UIManager::HandleEvent` only closes menus (e.g. sets visibility false) and does not duplicate logic that should stay in scenes.

---

## 3. Duplicate / redundant style and ini setup

**Where:** `Engine::Core_Init()` (style + `io.IniFilename`) and `ImGuiManager::Initialize()` (context + `ApplyCustomStyle()` + `io.IniFilename` in ImGuiManager).

**What:**  
- Two places set ImGui style and ini path; one of them operates on a context that is immediately replaced and leaked (see §1).  
- After fixing §1, only ImGuiManager should set style and ini (single source of truth).

**Fix:**  
- Same as §1: remove all ImGui context/io/style setup from `Core_Init()` so ImGuiManager is the only owner of context creation, ini path, and style.

---

## 4. Manual ImGui IO update vs backend

**Where:** `Engine::Core_Update()` (lines 286–306) and `Engine::Core_Event()` (line 241).

**What:**  
- `Core_Event()` uses `ImGui_ImplSDL2_ProcessEvent(event)` (imgui_impl_sdl platform layer).  
- There is no `ImGui_ImplSDL2_Init` or `ImGui_ImplSDL2_NewFrame`; rendering uses `ImGuiSDL::Initialize` / `ImGuiSDL::Render`.  
- So: platform input comes from imgui_impl_sdl (ProcessEvent only), while render is ImGuiSDL; mouse/keyboard are also manually set in `Core_Update()`: `io.DeltaTime`, `io.MousePos`, `io.MouseDown[0/1]`.  
- `io.DeltaTime` is hardcoded to `1.0f/60.0f` instead of actual frame time.  
- Mix of ProcessEvent + manual io is fragile: if someone adds ImGui_ImplSDL2_NewFrame later, input could be double-fed or overwritten.

**Fix (pick one approach):**  
- **Option A:** Rely on manual io only: remove `ImGui_ImplSDL2_ProcessEvent` and document that input is fed in `Core_Update`; set `io.DeltaTime` from real frame delta (e.g. from the same place you pass frame time to DebugUI).  
- **Option B:** Use imgui_impl_sdl for input only: call `ImGui_ImplSDL2_Init(Window::GetWindow())` once (e.g. in ImGuiManager after context create), and `ImGui_ImplSDL2_NewFrame(Window::GetWindow())` inside `ImGuiManager::BeginFrame()`; then remove manual `io.MousePos`, `io.MouseDown`, and possibly `io.DeltaTime` from `Core_Update` if the backend sets them (keep `io.DeltaTime` from engine if the backend doesn’t set it).  
- In both cases, set `io.DeltaTime` from actual frame time, not a constant.

---

## 5. getFrameTimes() still uses erase(begin())

**Where:** `Engine::Core_Update()` (lines 333–335).

**What:**  
- `getFrameTimes()` now returns `std::deque<float>&`, but the code still does `FrameTimes.erase(FrameTimes.begin())` when size > 100.  
- For deque, `pop_front()` is O(1) and matches the rest of the frame-time logic (render/logic use `pop_front()`).

**Fix:**  
- Replace with:  
  `if (FrameTimes.size() > 100) FrameTimes.pop_front();`

---

## 6. BeginModalDialog does not open the popup

**Where:** `ImGuiManager::BeginModalDialog()` (ImGuiManager.cpp).

**What:**  
- The function calls `ImGui::BeginPopupModal(name, open, flags)` but never `ImGui::OpenPopup(name)`.  
- Modals require `OpenPopup` to be called (once) before `BeginPopupModal` can succeed.  
- So the helper does not implement “begin modal” by itself; callers must remember to call `OpenPopup` first, which is easy to forget and inconsistent with the name.

**Fix:**  
- Either: (a) Rename to something like `BeginPopupModalOnce` and document that `OpenPopup` must be called by the caller, or (b) Add an overload or a separate helper that does `OpenPopup` + `BeginPopupModal` when the modal should be shown (e.g. when a “show” flag is true), so the common case is one call.

---

## 7. RenderImGui scene order

**Where:** `SceneManager::RenderImGui()` iterates over `_scenes` (map) and calls `scene->RenderImGui()` for each.

**What:**  
- Order of iteration is unspecified (e.g. map order); so draw order of overlay windows (Options, Help, LoadGame, MainMenu, GameScene) is not guaranteed.  
- If two scenes open ImGui windows, which one is on top can vary or be wrong (e.g. Options should be above MainMenu).

**Fix:**  
- If you need a defined stacking order, render in a fixed order, e.g.:  
  - First: game/menu background scenes (e.g. MainMenu, GameScene).  
  - Then: overlay scenes (Options, Help, LoadGame) in a defined order (e.g. Options, Help, LoadGame).  
- Implement by iterating over a fixed list of scene names or a dedicated “overlay” list instead of the full unordered map.

---

## 8. GameScene pause/death popups: OpenPopup every frame

**Where:** `GameScene::RenderImGui()` (pause and death modals).

**What:**  
- Code calls `ImGui::OpenPopup("Pause")` and `ImGui::OpenPopup("Player Death")` every frame while the condition is true.  
- ImGui allows this (re-opening or no-op when already open), but it’s redundant and can make behavior harder to reason about.

**Fix:**  
- Optional cleanup: call `OpenPopup` once when the state *becomes* true (e.g. when pause is set or player just died), and rely on `BeginPopupModal` on subsequent frames until the popup is closed.  
- If you keep the current pattern, document that you intentionally re-call `OpenPopup` every frame for simplicity.

---

## 9. ImGuiManager and window resize

**Where:** `ImGuiManager::Initialize(renderer, width, height)` and `ImGuiSDL::Initialize(renderer, windowWidth, windowHeight)`.

**What:**  
- If the window is resized, ImGuiSDL (and possibly ImGui backend) may need updated display size.  
- It’s unclear whether this is handled (e.g. in `RepositionGuiAtResChange` or elsewhere).  
- Wrong display size can cause incorrect layout or input (e.g. mouse coordinates).

**Fix:**  
- Ensure on resize you either: (a) call a resize path that updates ImGui display size (and any ImGuiSDL resize if it exists), or (b) re-call `ImGuiManager::Initialize(...)` with new width/height if the API supports reinit.  
- Document where resize is handled so future changes don’t break it.

---

## 10. Summary table

| # | Severity   | Issue                               | Location                          |
|---|------------|-------------------------------------|-----------------------------------|
| 1 | Critical   | Double CreateContext + context leak| Engine::Core_Init, ImGuiManager   |
| 2 | Medium     | UIManager::HandleEvent never called | Engine::Core_Event                |
| 3 | Medium     | Duplicate style/ini (after fixing 1)| Engine::Core_Init                 |
| 4 | Medium     | Manual io + hardcoded DeltaTime     | Engine::Core_Update, BeginFrame  |
| 5 | Low        | FrameTimes erase vs pop_front       | Engine::Core_Update              |
| 6 | Low        | BeginModalDialog doesn’t open       | ImGuiManager                      |
| 7 | Low        | RenderImGui scene order undefined   | SceneManager::RenderImGui         |
| 8 | Minor      | OpenPopup every frame (pause/death) | GameScene::RenderImGui            |
| 9 | Check      | Resize / display size update        | ImGuiManager / window resize path |

---

**Recommended order of fixes:**  
1) Remove duplicate ImGui context and style from `Core_Init()` (§1 and §3).  
2) Call `UIManager::HandleEvent(event)` from `Core_Event()` (§2).  
3) Use `pop_front()` for `FrameTimes` (§5).  
4) Set `io.DeltaTime` from real frame time and, if possible, align input source (manual vs imgui_impl_sdl) (§4).  
5) Then address modal helper (§6), scene order (§7), and popup/resize behavior (§8–9) as needed.
