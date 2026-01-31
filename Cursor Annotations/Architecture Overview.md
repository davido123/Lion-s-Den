# Lion's Den — Architecture Overview

Short reference after Game Loop Improvements (Days 1–5).

---

## Entry & Loop

- **main.cpp** — Creates `Engine`, calls `engine.Start()`. No game globals. Virtual hooks (`OnInit`, `OnEvent`, `OnUpdate`, `OnRender`, `OnCleanUp`, `OnRenderDebugGui`) are implemented in main and delegate to **Game** and **SceneManager**.
- **Engine** — Core loop: `Core_Init` → while(!quit) { render + track time; event; fixed-step updates (Core_Update, DeleteObjects) }. Uses `SDL_Event` on stack; frame times in **DebugUI** are `std::deque<float>` (O(1) pop_front). `TrackRenderTime` / `TrackLogicTime` helpers. No `_DeleteCandidates` (uses `Object::DeleteCandidates`).

---

## Game & Scenes

- **Game** — Singleton. Holds player, map, GUI widgets, save/load, menu state. Callbacks: `OnGameInit`, `OnGameEvent`, `OnGameUpdate`, `OnGameRender`, `OnGameCleanup`, `OnRenderDebugGui`. Still used in parallel with scenes (dual system).
- **SceneManager** — Singleton. Owns scenes by name; `SetActiveScene(name)`; forwards `HandleEvent`, `Update`, `Render`, `RenderImGui` to active scene.
- **Scene** — Base: `Enter`, `Exit`, `HandleEvent`, `Update`, `Render`, `RenderImGui()` (virtual, default empty), `GetName`, `SetEngine`.
- **Scenes** — MainMenuScene, GameScene, OptionsScene, HelpScene, LoadGameScene. GameScene uses Game getters for player/map/UI; pause can switch to MainMenu.

---

## Events & UI

- **EventSystem** — Singleton. `Subscribe(name, callback)` → id, `Unsubscribe(name, id)`, `Emit(name, data)`, `Clear`. Payload structs: PlayerDeathEvent, MonsterKilledEvent, HealthChangedEvent, etc.
- **ImGuiManager** — Singleton. Init, BeginFrame/EndFrame/Render, style, centered/modal window helpers, settings.
- **UIManager** — Singleton. Options/Help/LoadGame visibility; Open/Close; ESC handling; `Initialize`/`Shutdown`.

---

## Components

- **Component** — Base: `Object* _owner`, `_enabled`; virtual `OnUpdate`, `OnRender`, `OnEvent`.
- **HealthComponent** — Health/max, SetHealth, TakeDamage, Heal, IsDead; emits HealthChangedEvent when owner is Player.
- **MovementComponent** — Stub (maxSpeed, moveTarget); OnUpdate empty for future movement logic.
- **Player** — Uses `_healthComponent` (Life/MaxLife delegated), `_movementComponent`; death and stats emit events; GameScene subscribes to PlayerDeath, MonsterKilled, HealthChanged and updates UI / handles death.

---

## File Layout (relevant)

- **Lion's Den/** — main, Engine, DebugUI; Game/, Scenes/, Systems/, Components/, GUI/, Core/, Render/, IO/, ImGui/, etc.
- **Cursor Annotations/** — Implementation Plan, ImGui Plan, Engine Analysis, Architecture Overview, recollections.

---

*Generated after Day 5 polish; reflects refactor-game-loop branch.*
