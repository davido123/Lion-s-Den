#ifndef GAME_H_
#define GAME_H_

#include "Engine.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "Core/Vec2.h"
#include "Core/Resources.h"
#include "Core/Object.h"
#include "Core/Collider.h"
#include "Core/Audio.h"

#include "GUI/Text.h"
#include "GUI/Widgets/Box.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/StatsBar.h"
#include "GUI/Widgets/Inventory.h"
#include "GUI/Widgets/CharacterStats.h"
#include "GUI/Window.h"
#include "GUI/GUI.h"

#include "Common/FPScounter.h"
#include "Common/sys.h"
#include "Render/Drawer.h"
#include "Render/Camera.h"
#include "Render/Surface.h"

#include "Player.h"
#include "Map.h"
#include "Monster.h"
#include "Item.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_sdl.h"
#include "ImGui/imgui_struct.h"

#include "Settings.h"
#include "DebugUI.h"
#include "IO/Mouse.h"
#include "IO/Keyboard.h"

#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <io/JsonBase.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace rapidjson;
using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace std;

enum GameState {
	PAUSE = 0,
	NEW_GAME,
	LOAD_GAME,
	SAVE_GAME,
	MAIN_MENU,
	PAUSE_MENU,
	PLAY
};

// Forward declaration
class Engine;

class Game {
public:
	static Game& GetInstance();
	
	// Set Engine reference (called from Engine::OnInit)
	void SetEngine(Engine* engine) { _engine = engine; }
	
	// Game lifecycle
	void Initialize();
	void Shutdown();
	
	// Game state callbacks (called from Engine)
	void OnGameInit();
	void OnGameEvent(SDL_Event* event, const Uint8* keyboardState);
	void OnGameUpdate();
	void OnGameRender();
	void OnGameCleanup();
	
	// Game state management
	GameState GetState() const { return _gameState; }
	void SetState(GameState state) { _gameState = state; }
	
	// Save/Load
	void SaveGame();
	void LoadGame();
	void NewGame(const std::string& saveName);
	
	// UI Menu rendering (moved to scenes - kept for backward compatibility during migration)
	void OnRenderDebugGui();
	void RepositionGuiAtResChange();
	
	// Access to game objects (for now, will be refactored later)
	Player* GetPlayer() const { return player; }
	Map* GetMap() const { return map2; }
	StatsBar* GetStatsBar() const { return GuiBox_StatsBar; }
	Inventory* GetInventory() const { return Player_Inventory; }
	Object* GetLayerGui() const { return layer_gui; }
	Object* GetLayerPlayer() const { return layer_player; }
	bool IsDrawCollisions() const { return DrawCollisions; }
	
	// ImGui settings management (public for scene access)
	void LoadImGuiSettings();
	void SaveImGuiSettings();
	
private:
	Game() : _engine(nullptr) {}
	~Game() = default;
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	
	Engine* _engine;  // Reference to Engine for accessing public members
	GameState _gameState = MAIN_MENU;
	std::string savename;
	
	// Game objects (moved from globals)
	Object* layer_gui;
	Object* layer_player;
	
	// GUI Elements
	Box* GuiBox_MainMenu;
	Box* GuiBox_MenuOptions;
	StatsBar* GuiBox_StatsBar;
	Inventory* Player_Inventory;
	
	Button* GuiButton_MainMenu_Start;
	Button* GuiButton_MainMenu_Options;
	Button* GuiButton_MainMenu_ExitGame;
	Button* GuiButton_MainMenu_NewGame;
	Button* GuiButton_MainMenu_LoadGame;
	
	// Game elements
	Player* player;
	Map* map2;
	
	// Singletons
	Settings* settings;
	DebugUI* debugui;
	
	// UI state flags
	bool ShowDebug = false;
	bool DrawCollisions = false;
	bool ShowOptions = false;
	bool ShowLoad = false;
	bool ShowHelp = false;
	bool muteMusic = false;
	
	// Helper methods
	void SetupMainMenu();
	void SetupGame();
	void CleanupGame();
	void HandleMainMenuEvents(SDL_Event* event);
	void HandlePlayStateEvents(SDL_Event* event);
	void HandlePauseMenu();
	void HandlePlayerDeath();
	
};

#endif
