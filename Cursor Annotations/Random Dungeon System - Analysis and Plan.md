# Random Dungeon System – Analysis and Implementation Plan

## 1. Current Game World – How It Works

### 1.1 Map creation and loading

- **Source:** Single static map from Tiled: `Data/Maps/Map.json` loaded via **Tileson** (`tson::Tileson::parse()`).
- **Structure:** One `tson::Map`; layers by name: `"Floor"`, `"Floor2"`, `"Walls"`, `"Enemies"`.
- **Rendering:** `Map::DrawLayer(layerName)` / `DrawLayerOrderNR("Walls", 0|1)` – iterates tile objects, draws with `Dungeon_Tileset.png` (32×32), scaled by `Map::SCALED_TILE_SIZE` (32).
- **Tiles:** Tile ID from Tiled; collision comes from the tileset’s **object group** per tile ID (`TileCollider` = pos, size, id). `GetCollisionBoxes()` builds a list of those; `RegisterCollisionBoxes()` creates an `Object` collider per **placed tile** on the map and registers it with `Collider`.
- **No runtime generation:** Map is load-only. `SaveJson()` returns false; there is no in-memory tile grid API – only Tileson’s layer/tile iteration.

### 1.2 Map “editing”

- **No editor in-game.** Map is authored in Tiled and saved as JSON. At runtime the map is read-only: no tile placement/removal, no procedural generation.

### 1.3 Monsters

- **Spawn:** `Map::SpawnMonsters()` reads the **"Enemies"** layer. For each tile object, position (in tile units) is converted to pixels; monster type = `tile->getObjectgroup().getId()` used as index into **`Monster::MonsterTypes`**.
- **Types (hardcoded in `SaveMonsterTypes()`):** 0=Physical, 1=Fire, 2=Cold, 3=Lightning, 4=Boss. Each type has Life, Armour, resistances, damage ranges.
- **Lifecycle:** New `Monster()` → `SetPos()` → `SetParams(MonsterTypes[id])` → Collider register → push to `Monster::MonsterList`. Same list is used by both `Game` and `GameScene` for update/render.
- **Death:** In `Monster::OnUpdate()`, when `Life <= 0`: `isDead = true`, emit `MonsterKilledEvent`, then `DropItem()` (1/3 chance: random `Item` with type, rarity, modifiers), stored in `GroundItems` (one `Slot`). No “boss” flag or special behaviour beyond the “Boss” type name.

### 1.4 Items and progression

- **Items:** `Item` with `Modifier`s, `item_rarity`, `item_type` (equipment slots). `DropItem()` picks random type/rarity and calls `AddRandomModifier()` per rarity.
- **Progression:** No dedicated system. Progression is implicit: better gear from killing monsters; player stats/inventory already exist. No “dungeon level” or “floor difficulty” driving drops.

### 1.5 Save/load

- **Save (new):** Full snapshot of current level: dungeon layout (or seed + floor index), player, inventory, monsters, ground items, current map id (Village vs dungeon floor N), boss dead / portal enabled flags. Load restores **everything as when saved** (same layout, same entities).

---

## 2. Confirmed Design Decisions

Decisions below; full detail in Phases 0–4 and §6–7.

| **Room size** | Random per room (min/max bounds). |
| **Room count** | By floor level: first floor ~5 rooms; each next floor more (e.g. baseRooms + floorLevel). |
| **Persistence** | Save = full snapshot; load restores everything as when saved. |

1. **Dungeon size and shape**  
   Should each “level” be one contiguous dungeon (e.g. grid of rooms/corridors), or multiple disconnected zones? Preferred size (e.g. room count, total tile size)?

2. **Safe zone**  
   One fixed “starting” room (no enemies, no spawns) where the player appears and can return to, or should the “safe” area be the whole first room and never spawn enemies there?

3. **Boss and “final room”**  
   One boss per level in one designated “final” room, and the “next level” exit appears only after that boss is dead – correct? Should the exit be a door, a staircase, or an interactable object?

| **Difficulty** | **Global** setting: Easy / Medium / Hard. Single **global difficulty modifier** applied to enemy health, damage, and skills. |

5. **Progression integration**  
   “Progression system depending on player items from defeating enemies” – do you want: (a) drop tables that improve with player level or dungeon depth, (b) explicit “progression” stats (e.g. dungeon level, score) that gate content, or (c) only the existing item/rarity/modifier system with configurable tables per zone/level?

| **New game start** | New game starts in a **static Village** (hub). Village is **not generated**; one static map. Player can **trade** and **store items in chest(s)**. Village is the only non-generated area; from there player enters the dungeon (first generated floor). |

---

## 3. High-Level Architecture of the New System

- **DungeonGenerator** – Builds a single “level”: layout (rooms, corridors), tile data, spawn zones (safe, normal, boss), and metadata (which room is start, which is boss, exit tile/object).
- **DungeonConfig** – Tunable parameters: room count/size, enemy count per room or total, difficulty curve, boss room rules, seed (optional).
- **Integration with existing Map** – Either:
  - **A)** Generate a Tileson-compatible structure (e.g. JSON or in-memory `tson::Map`-like) and feed it to the existing `Map` (minimal change to rendering/collision), or  
  - **B)** Add a “generated” mode to `Map`: accept a tile grid + layers from the generator and draw/register collision from that (requires Map to support a second data path).
- **Spawn system** – Separate from Tiled “Enemies” layer: a **SpawnManager** (or generator output) decides: count and types of enemies per room, one boss in the boss room, and optionally safe-zone (no spawns). Uses existing `Monster::MonsterTypes` and `MonsterList`; can later plug into difficulty/progression.
- **Boss death and exit** – `MonsterKilledEvent` already exists. When the killed monster is the level boss, set a flag or emit “BossKilled”; game logic enables the “next level” exit (e.g. make a door interactable or show a staircase). Optionally emit “LevelComplete” for progression/UI.

---

## 4. Implementation Plan (Phased)

### Phase 0: Static Village (hub)

- **Village map:** One **static** Tiled map (e.g. `Data/Maps/Village.json`). **Not generated**; same map every new game. Only non-generated area in the game.
- **New game start:** New game always starts in the Village. Player spawns at a fixed position; Village has no enemies (safe hub).
- **Features:** **Trade** (e.g. NPC or shop UI to buy/sell items); **chest(s)** to store items (persist stored items in save when in Village, or when player leaves/returns).
- **Transition to dungeon:** From Village, player enters the dungeon (e.g. interact with "dungeon entrance" object) → load first generated floor (floor level 1, ~5 rooms), spawn player in Start room, run SpawnManager. When player uses portal on a floor, they go to next floor (or optionally add "return to Village" from dungeon Start room later).
- **Save/load:** When saving in Village, persist Village state (player position, inventory, chest contents, etc.). When saving in dungeon, persist full dungeon snapshot (see §1.5 / Phase 2–4).

**Deliverables:** Village map (Tiled), Village scene or map id, trade UI or stub, chest storage (serialize with save), transition Village → dungeon floor 1.

---

### Phase 1: Dungeon layout and data model

**1.1 Tile / room representation**

- Define a **grid-based dungeon representation** independent of Tileson:
  - 2D grid of tile IDs (floor, wall, door, etc.) and optional “layer” (e.g. floor vs wall).
  - Room list: each room = rect + type (e.g. `Start`, `Normal`, `Boss`).
- Decide tile set: subset of existing `Dungeon_Tileset` IDs for floor/wall/door so existing rendering can be reused.

**1.2 DungeonGenerator (core algorithm)**

- **Input:** DungeonConfig (grid size, **room count from floor level**: first floor ~5 rooms, each next floor more; min/max **random room size**, seed).
- **Output:** Grid + list of rooms with types (start, normal, boss).
- **Algorithm (classic roguelike):**
  1. Place rooms (random rects, non-overlapping or minimal overlap).
  2. Mark one room as Start (safe zone), one as Boss.
  3. Build a spanning tree (e.g. MST) or graph of room centers; carve corridors (L-shaped or straight) to connect rooms.
  4. Write floor/wall/door tiles into the grid; ensure Start has no enemy spawns; Boss room marked for one boss spawn and later “exit” tile.
- **Determinism:** Use a single RNG (e.g. `std::mt19937`) seeded by DungeonConfig so the same seed = same layout.

**1.3 Map integration (generated path)**

- **Option A – Generate Tiled JSON:** Implement a writer that produces a valid `.tmj`/`.json` with one layer per “layer name” (Floor, Floor2, Walls) and an Enemies layer (can be empty; spawns done in code). Then `Map::LoadJson(generatedPath)` or load from memory. **Pros:** no change to Map. **Cons:** need to implement Tiled JSON format and possibly temp file or buffer.
- **Option B – Map supports “generated” mode:** Add to `Map` something like `LoadFromGrid(width, height, floorLayer, wallLayer, collisionData)` that fills internal structures (or a second `tson::Map`-like structure) and uses the same `DrawLayer` / `GetCollisionBoxes` / `RegisterCollisionBoxes` flow. **Pros:** no file format; direct. **Cons:** Map must be refactored to support two sources (Tiled vs generated).

**Recommendation:** Start with **Option A** (generate JSON) to avoid touching Map internals; if the format becomes a bottleneck, add Option B later.

**Deliverables:** DungeonConfig, DungeonGenerator (grid + rooms), tile set constants, and either a JSON generator or a Map API for generated data.

---

### Phase 2: Safe zone, boss room, and exit

**2.1 Safe zone**

- Generator always sets one room as `Start`; that room’s tiles are marked (e.g. in room metadata or a “no-spawn” zone list).
- Player spawn position: center (or fixed offset) of Start room, in pixels.
- SpawnManager (Phase 3) will skip spawning enemies in the Start room.

**2.2 Boss room and exit**

- Generator marks one room as `Boss`. SpawnManager will spawn exactly one boss there (Phase 3).
- Add an **exit tile or object** in the Boss room (e.g. specific tile ID “stairs” or “portal”). Initially **disabled** (invisible or non-interactable).
- On **MonsterKilledEvent:** if the killed monster is the level boss (e.g. `monsterType == 4` and flagged as “level boss”, or a dedicated `isBoss` flag), set “boss dead” for current level and **enable the exit** (show stairs / set interactable). Optionally emit `LevelComplete` or `BossKilled` for UI/progression.

**2.3 “Next level” flow**

- When the player interacts with the exit (e.g. overlap + key or click): advance “current level” (e.g. `Game` or a LevelManager holds `currentDungeonLevel`), optionally increment difficulty, then **generate the next dungeon** (new layout, new spawns), respawn player in the new Start room, and run SpawnMonsters equivalent for the new level.

**Deliverables:** Boss flag or type check, exit tile/object, “boss dead → exit enabled” logic, “use exit → generate next level” flow.

---

### Phase 3: Enemy placement and difficulty

**3.1 SpawnManager (or generator-driven spawn list)**

- **Input:** Dungeon layout + room list + DungeonConfig (enemy count, difficulty params).
- **Output:** List of spawns: (position, monster type index, optional “isBoss”).
- **Rules:**
  - No spawns in Start room.
  - Boss room: exactly one spawn, type = Boss (e.g. `MonsterTypes[4]`), and mark that instance as “level boss” so killing it enables the exit.
  - Other rooms: semi-random count per room (e.g. 0–2 per room, or total cap with random distribution). Use RNG (same seed as layout for reproducibility).

**3.2 Configurable amount and difficulty**

- **DungeonConfig** (or a separate CombatConfig):
  - `min/maxEnemiesPerRoom` or `totalEnemyCap`;
  - `difficultyLevel` or `depth` (integer) used for scaling;
  - Optional: per-room “danger” (e.g. distance from start) for local scaling.
- **Difficulty scaling:** For each non-boss spawn, compute a scale factor from depth/distance. Apply to:
  - **Type choice:** Weight toward harder MonsterTypes as depth increases (e.g. more Fire/Cold/Boss-like).
  - **Stats:** Scale Life, Armour, damage (PhysicalDamage, etc.) by a multiplier from config (e.g. `1 + depth * 0.1`). This may require a copy of params from `MonsterTypes[id]` and then scale before `SetParams` or extend `SetParams` to accept a scale factor.

**3.3 Integration with existing systems**

- **Monster creation:** Same as now: `new Monster()`, `SetPos()`, `SetParams(MonsterTypes[type])`, Collider register, push to `MonsterList`. If scaling is added, add a step that applies stat multipliers to the monster after `SetParams`.
- **Progression:** Reserve hooks: e.g. `GetCurrentDepth()` or `GetPlayerLevel()` used by SpawnManager or drop tables so that later you can drive “amount” and “difficulty” and “drop quality” from player items/level (Phase 4).

**Deliverables:** SpawnManager, configurable enemy count and distribution, optional stat scaling, boss marked and wired to exit.

---

### Phase 4: Progression and items

**4.1 Progression system (minimal)**

- Introduce a **ProgressionContext** (or use existing Game/Scene state): e.g. `currentDungeonLevel`, `playerLevel`, or “power score” derived from equipment. This will be read by:
  - SpawnManager (enemy count, difficulty scaling),
  - Drop tables (item quality, rarity weights).
- No need to implement full “skill tree” in this phase – only the data that difficulty and drops depend on.

**4.2 Drop tables and items**

- **Monster::DropItem()** today: 1/3 chance, random type/rarity/modifiers. Extend so that:
  - **Drop tables** that improve with **depth/level**: drop chance and rarity weights depend on `ProgressionContext` (e.g. deeper floor = higher magic/rare chance). Configurable tables (e.g. per floor range or per monster type; boss can have guaranteed rare).
- Keep existing `Item`/`Modifier`/rarity system; only the **inputs** to the random choices become configurable (drop tables by depth/level).

**4.3 Integration**

- When a monster is killed, progression can be updated (e.g. “monsters killed this run” or “experience”); when the player equips items, `ProgressionContext` can reflect that. SpawnManager and drop logic read from `ProgressionContext` so that “player items from defeating enemies” feed back into harder encounters and better drops.

**Deliverables:** ProgressionContext (or equivalent), configurable drop chance/rarity by context, optional stat scaling by depth, hooks for future expansion.

---

## 5. File / Class Layout (suggested)

- **Dungeon/** (or **DungeonGen/**)
  - `DungeonConfig.h/.cpp` – config struct and defaults.
  - `DungeonGenerator.h/.cpp` – layout algorithm, grid + rooms.
  - `DungeonTileSet.h` – tile ID constants (floor, wall, door, stairs, etc.).
  - `TiledJsonWriter.h/.cpp` (if Option A) – build Tiled-format JSON from grid + rooms.
- **SpawnManager.h/.cpp** – spawn list from layout + config; calls into existing Monster creation; marks boss.
- **ProgressionContext.h/.cpp** (Phase 4) – current level/depth, optional player power; used by SpawnManager and DropItem.
- **Map** – either unchanged (load generated JSON) or add `LoadFromGenerated(...)` (Option B).
- **Game/GameScene** – hold current level index, call generator when starting new game or when player uses “next level” exit; set player position to Start room; call SpawnManager instead of (or in addition to) `Map::SpawnMonsters()` when using generated dungeons.

---

## 6. Order of implementation (summary)

0. **Phase 0:** Static Village map, new game start in Village, trade + chests, Village to dungeon floor 1.
1. **Phase 1:** DungeonConfig, grid + rooms data structure, DungeonGenerator (rooms + corridors), tile set, then Tiled JSON output OR Map “generated” API.
2. **Phase 2:** Wire Start room to player spawn; add Boss room and exit tile/object; on boss death enable exit; on exit use → generate next level and re-enter.
3. **Phase 3:** SpawnManager: no spawn in Start, one boss in Boss room, semi-random others; configurable count and difficulty scaling; integrate with existing Monster/MonsterList.
4. **Phase 4:** Drop tables by depth/level; ProgressionContext; save = full snapshot, load = restore exactly.

---

## 7. Dependencies and risks

- **Tileson:** If we generate JSON, we must match the exact layer/tile format your `Map` and tileset expect (layer names, tile IDs, object group IDs for collision). Need to inspect `Map.json` or a sample to confirm.
- **Game vs GameScene:** Both use `Monster::MonsterList` and a map. Decide which path “owns” the dungeon (likely Game for now, with GameScene using the same data when in play). Avoid duplicate spawn or double map load.
- **Save/load:** Save full snapshot (seed + floor index, or serialized layout; player, inventory, monsters, ground items, chests, map id, flags). Load restores everything so state is identical after restart.
- **Village:** Ensure Village is loaded as a distinct map id (e.g. `currentMapId == Village`) so new game and "return to Village" (if added) load the static map; dungeon floors load generated maps.
