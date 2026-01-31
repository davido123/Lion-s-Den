# refactor-game-loop — Code Modifications Recollection

**Branch**: `refactor-game-loop`

Summary of code changes on this branch (from repo state and annotations).

---

## Modified Files

| Path | Summary of changes |
|------|--------------------|
| **Lion's Den/main.cpp** | Game logic removed; delegates to `Game`; initializes SceneManager, UIManager; registers MainMenu, Game, Options, Help, LoadGame scenes; sets initial scene "MainMenu". |
| **Lion's Den/Engine.cpp** | ImGuiManager init/shutdown in Core_Init/Core_CleanUp; SceneManager in Core_Event, Core_Update, Core_Render; ImGuiManager BeginFrame/EndFrame/Render; `SceneManager::RenderImGui()` after game render. |
| **Lion's Den/Engine.h** | Kept engine core; virtual hooks for OnInit/OnEvent/OnUpdate/OnRender/OnRenderDebugGui/OnCleanUp. |
| **Lion's Den/DebugUI.cpp** | Adjusted for new loop / ImGui usage (if any). |
| **Lion's Den/GUI/Widget.cpp** | Tweaks for scene/UI integration. |
| **Lion's Den/GUI/Widgets/Inventory.h** | Interface/visibility for scene access. |
| **Lion's Den/GUI/Widgets/StatsBar.h** | Interface/visibility for scene access. |
| **Lion's Den/Lion's Den.vcxproj** | Added Game/, Scenes/, Systems/, ImGuiHelper; updated includes. |
| **.vscode/** | c_cpp_properties.json, launch.json, settings.json, tasks.json, MSVC_INTEGRATION.md. |

---

## New Directories & Files

### Game/
- **Game.h** — Singleton Game; lifecycle (Initialize, Shutdown); callbacks (OnGameInit, OnGameEvent, OnGameUpdate, OnGameRender, OnGameCleanup); GameState enum; save/load/new; getters for scene use (e.g. GetStatsBar, GetInventory, GetLayerGui, GetLayerPlayer); UI flags; helpers (SetupMainMenu, SetupGame, etc.).
- **Game.cpp** — Implements above; still holds main menu/gameplay logic in parallel with scenes (dual system).

### Scenes/
- **Scene.h** — Base scene: Enter(), Exit(), HandleEvent(), Update(), Render(), RenderImGui() (virtual, default empty), GetName(), SetEngine().
- **MainMenuScene** — Menu UI (Box, Buttons); event handling; state sync with Game.
- **GameScene** — Uses Game getters for Player, Map, UI; input (I, C, mouse); update (monsters, camera, pause); render map/entities/GUI; pause menu with scene switch to MainMenu; RenderImGui() for pause/death.
- **OptionsScene** — Options menu ImGui in RenderImGui().
- **HelpScene** — Help menu ImGui in RenderImGui().
- **LoadGameScene** — Load game menu ImGui in RenderImGui().

### Systems/
- **SceneManager** — Singleton; RegisterScene (unique_ptr), SetActiveScene, GetScene, GetCurrentScene; HandleEvent, Update, Render, RenderImGui(); holds scenes by name.
- **ImGuiManager** — Singleton; Initialize(renderer, size), Shutdown; BeginFrame, EndFrame, Render; ApplyCustomStyle; BeginCenteredWindow, BeginModalDialog; SaveSettings, LoadSettings.
- **UIManager** — Singleton; ShowOptions/ShowHelp/ShowLoadGame + Is*Visible; OpenOptions/OpenHelp/OpenLoadGame, CloseAllMenus; HandleEvent (e.g. ESC); Initialize, Shutdown.

### GUI/
- **ImGuiHelper.cpp/.h** — Reusable ImGui helpers (centering, buttons, etc.).

### Root / docs
- **BUILD_INSTRUCTIONS.md** — How to build the project.

---

## Architectural Summary

- **Dual system**: Game class and SceneManager both run; Game still owns gameplay objects; scenes reference them via Game getters.
- **Main loop**: Core_Event → SceneManager::HandleEvent + OnEvent (Game); Core_Update → SceneManager::Update + OnUpdate (Game); Core_Render → game render, then ImGuiManager BeginFrame → SceneManager::RenderImGui() → OnRenderDebugGui → ImGuiManager EndFrame/Render.
- **Scene switching**: e.g. GameScene pause menu calls `SceneManager::GetInstance().SetActiveScene("MainMenu")`.
- **ImGui**: Per-scene RenderImGui(); Options/Help/LoadGame as separate scenes; UIManager for menu visibility; ImGuiManager for frame lifecycle and style.

---

## Deleted (from git status)

- `Cursor Annotations/Implemmentation plan` (typo name; content restored elsewhere).
- `setup_git_backup.ps1`.

---

*Recollection based on repo layout, main.cpp, Engine.cpp/h, and Cursor Annotations (Day 2/3, ImGui plan).*
