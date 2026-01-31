#include "Game.h"
#include "../Scenes/MainMenuScene.h"
#include "../Systems/SceneManager.h"
#include <fstream>
#include <algorithm>
#include <sstream>

// Singleton implementation
Game& Game::GetInstance() {
	static Game instance;
	return instance;
}

void Game::Initialize() {
	// Initialization will be done in OnGameInit
}

void Game::Shutdown() {
	// Cleanup will be done in OnGameCleanup
}

void Game::OnGameInit() {
	//Update Item Modifiers XML file from code
	Item::FillModifiersFile();
	
	//Update Monster Data XML file from code
	Monster::SaveMonsterTypes();

	//Load data of all Save files from Data/Saves
	if (_engine) {
		_engine->saves.clear();
		std::filesystem::create_directories("Data/Saves");
		try {
			for (const auto& file : std::filesystem::directory_iterator("Data/Saves/")) {
				if (file.is_directory()) {
					std::string savename = file.path().filename().string();
					_engine->saves.push_back(savename);
					cout << "Found save:" << savename << endl;
				}
			}
		} catch (const std::filesystem::filesystem_error&) {
			// Directory missing or unreadable
		}
	}

	//Load Game Background music
	Audio *a = new Audio("Data/Sounds/test.wav",true);
	//a->Play(1);
	
	//Initialise settings and debugui singleton classes
	settings = Settings::GetInstance("1");
	debugui = DebugUI::GetInstance("1");
	
	// Load ImGui settings if they exist
	LoadImGuiSettings();
	
	
	//Load map file
	map2 = new Map;
	map2->LoadJson("Data/Maps/Map.json");
	map2->GetCollisionBoxes();
	map2->RegisterCollisionBoxes();

	//Initializing layers
	if (_engine) {
		_engine->AddLayer();
		_engine->AddLayer();
		_engine->AddLayer();

		layer_player = _engine->GetRootAtLayer(0);
		layer_player->_custom_render = true;
		layer_gui = _engine->GetRootAtLayer(1);
	}

	player = new Player();
	Collider::RegisterObject(player,player->GetColliderPos(),player-> GetColliderBox(),false);
	Collider::RegisterTarget(player,player->GetColliderPos(),player-> GetColliderBox());

	//Initializing Gui elements
	// Check if MainMenuScene exists and use it, otherwise use Game's own setup
	SceneManager& sceneManager = SceneManager::GetInstance();
	Scene* mainMenuScene = sceneManager.GetScene("MainMenu");
	if (mainMenuScene) {
		// MainMenuScene will handle its own setup
		// Game class will still manage other UI elements
	} else {
		// Fallback to Game's own menu setup
		SetupMainMenu();
	}

	/************Player Hud******************/
	GuiBox_StatsBar = new StatsBar(Vec2(0,0),Vec2(300,64));
	GuiBox_StatsBar->SetStyle("GUI/hud.png");
	layer_gui->Connect(GuiBox_StatsBar);
	GuiBox_StatsBar->Show(false);
	
	/***************Inventory****************/
	Player_Inventory = new Inventory(Vec2(80,40),Vec2(35,60), Resources::GetDefaultFont(),16);
	Player_Inventory->Show(false);
	layer_gui->Connect(Player_Inventory);
	GuiBox_StatsBar->ConnectPlayer(player);

	Player_Inventory->ConnectPlayer(player);
	layer_player->Connect(player);
}

void Game::SetupMainMenu() {
	/*****************MAIN MENU BOX*******************/
	GuiBox_MainMenu = new Box(85, 75, 20, 40, Resources::GetDefaultFont(), 16);
	GuiBox_MainMenu->SetLabel("MainMenu");
	layer_gui->Connect(GuiBox_MainMenu);

	/*****************MAIN MENU BUTTONS*******************/
	GuiButton_MainMenu_NewGame = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Play");
	GuiBox_MainMenu->Connect(GuiButton_MainMenu_NewGame);

	GuiButton_MainMenu_LoadGame = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Help");
	GuiBox_MainMenu->Connect(GuiButton_MainMenu_LoadGame);

	GuiButton_MainMenu_Options = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Options");
	GuiBox_MainMenu->Connect(GuiButton_MainMenu_Options);

	GuiButton_MainMenu_ExitGame = new Button(Vec2::ZERO, Vec2::ZERO, Resources::GetDefaultFont(), 16, "Quit Game");
	GuiBox_MainMenu->Connect(GuiButton_MainMenu_ExitGame);

	GuiBox_MainMenu->RecalcCentered();
	/*****************MAIN MENU BUTTONS END*******************/
	/*****************MAIN MENU BOX*******************/
}

void Game::OnGameEvent(SDL_Event *event, const Uint8 *keyboardState) {
	if (event->type == SDL_KEYDOWN) {
		if (Keyboard::isKeyDown(KEY_ESCAPE)) {
			Engine::Pause = true;
		}
		if (Keyboard::isKeyDown(KEY_GRAVE)) {
			ShowDebug = !ShowDebug;
		}
	}
	
	if (_gameState == MAIN_MENU) {
		HandleMainMenuEvents(event);
	}
	
	if (_gameState == PLAY) {
		HandlePlayStateEvents(event);
	}
}

void Game::HandleMainMenuEvents(SDL_Event* event) {
	// Check if MainMenuScene is active and handle events through it
	SceneManager& sceneManager = SceneManager::GetInstance();
	Scene* activeScene = sceneManager.GetActiveScene();
	
	if (activeScene && activeScene->GetName() == "MainMenu") {
		// MainMenuScene handles its own events
		// Sync state flags for backward compatibility
		MainMenuScene* mainMenuScene = dynamic_cast<MainMenuScene*>(activeScene);
		if (mainMenuScene) {
			// Sync state flags - but only update if MainMenuScene says they're true
			// This prevents stale state from reopening closed menus
			if (mainMenuScene->IsOptionsVisible()) {
				ShowOptions = true;
			} else if (!mainMenuScene->IsOptionsVisible() && ShowOptions) {
				// If MainMenuScene says options is closed, respect that
				ShowOptions = false;
			}
			if (mainMenuScene->IsLoadVisible()) {
				ShowLoad = true;
			} else if (!mainMenuScene->IsLoadVisible() && ShowLoad) {
				ShowLoad = false;
			}
			if (mainMenuScene->IsHelpVisible()) {
				ShowHelp = true;
			} else if (!mainMenuScene->IsHelpVisible() && ShowHelp) {
				ShowHelp = false;
			}
		}
		return;
	}
	
	// Fallback to Game's own event handling
	if (Mouse::InWindow()) {
		if (!ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse) {
			if (Mouse::Pressed(MOUSE_LEFT)) {
				if (GUI::GetLastClicked() != nullptr) {
					if (GUI::GetLastClicked()->GetLabel() == "Quit Game") {
						if (_engine) {
							_engine->Stop();
						}
					}
					else if (GUI::GetLastClicked()->GetLabel() == "Options") {
						if (GuiBox_MainMenu) {
							GuiBox_MainMenu->Show(false);
						}
						ShowOptions = true;
					}
					else if (GUI::GetLastClicked()->GetLabel() == "Play") {
						ShowLoad = true;
						if (GuiBox_MainMenu) {
							GuiBox_MainMenu->Show(false);
						}
					}
					else if (GUI::GetLastClicked()->GetLabel() == "Help") {
						ShowHelp = true;
						if (GuiBox_MainMenu) {
							GuiBox_MainMenu->Show(false);
						}
					}
				}
			}
		}
	}
}

void Game::HandlePlayStateEvents(SDL_Event* event) {
	if (event->type == SDL_KEYDOWN) {
		if (Keyboard::isKeyDown(KEY_I)) {
			Player_Inventory->Show(!Player_Inventory->IsVisible());
			Player_Inventory->SetRenderItems(Player_Inventory->IsVisible());
		}
		if (Keyboard::isKeyDown(KEY_C)) {
			player->drawStatBox = !player->drawStatBox;
		}
	}
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		Player_Inventory->_mbtpressed = true;
		Player_Inventory->_mbtreleased = false;
	}
	if (event->type == SDL_MOUSEBUTTONUP) {
		Player_Inventory->_mbtpressed = false;
		Player_Inventory->_mbtreleased = true;
	}
}

void Game::OnGameUpdate() {
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;

	if (_gameState == PLAY) {
		for (auto i : Monster::MonsterList) {
			i->SetMoveTarget(player->GetPos());
		}
		Window::GetCamera()->CenterOnPlayer(player->GetPos(), player->GetSize(), map2->GetMapSizePixels());
	}
}

void Game::OnGameRender() {
	// When GameScene is active it renders game content; skip here to avoid double rendering
	Scene* active = SceneManager::GetInstance().GetActiveScene();
	if (active && active->GetName() == "Game") {
		return;
	}
	if (_gameState == PLAY) {
		map2->DrawLayer("Floor");
		map2->DrawLayer("Floor2");
		map2->DrawLayerOrderNR("Walls", 0);
		if (_engine) {
			_engine->GetRootAtLayer(0)->RenderChildren();
		}
		map2->DrawLayerOrderNR("Walls", 1);

		if (DrawCollisions) {
			Collider::DrawCollisions();
		}

		HandlePauseMenu();
		
		if (player->isDead) {
			HandlePlayerDeath();
		}
	}

	if (_engine) {
		_engine->GetRootAtLayer(1)->RenderChildren();
	}

	Drawer::RenderAll(Window::GetRenderer(), Window::GetCamera());
}

void Game::HandlePauseMenu() {
	if (Engine::Pause) {
		ImGui::OpenPopup("Pause");
		if (ImGui::BeginPopupModal("Pause", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Do you want to exit and save game?");
			ImGui::Separator();
			if (ImGui::Button("Yes", ImVec2(120, 0))) {
				SaveGame();
				Engine::Pause = false;
				_gameState = MAIN_MENU;
				if (GuiBox_MainMenu) GuiBox_MainMenu->Show(true);
				GuiBox_StatsBar->Show(false);
				Player_Inventory->Show(false);
				Player_Inventory->Clear();
				Player_Inventory->overlay->DisconnectAll();

				for (auto i : Monster::MonsterList) {
					layer_player->Disconnect(i);
					i->DeleteLater();
					Collider::UnregisterObject(i);
					Collider::UnregisterTarget(i);
				}
				player->StatsReset();

				Player_Inventory->overlay->DisconnectAll();
				Player_Inventory->Show(false);
				Player_Inventory->SetRenderItems(false);
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				Engine::Pause = false;
			}
			ImGui::EndPopup();
		}
	}
}

void Game::HandlePlayerDeath() {
	if (player->isDead) {
		ImGui::OpenPopup("Dead");
		if (ImGui::BeginPopupModal("Dead", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You are dead, exit to menu?");
			ImGui::Separator();
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Yes", ImVec2(120, 0))) {
				Engine::Pause = false;
				_gameState = MAIN_MENU;
				if (GuiBox_MainMenu) GuiBox_MainMenu->Show(true);
				GuiBox_StatsBar->Show(false);
				Player_Inventory->Show(false);
				Player_Inventory->Clear();
				Player_Inventory->overlay->DisconnectAll();

				for (auto i : Monster::MonsterList) {
					layer_player->Disconnect(i);
					i->DeleteLater();
					Collider::UnregisterObject(i);
					Collider::UnregisterTarget(i);
				}
				player->StatsReset();

				Player_Inventory->overlay->DisconnectAll();
				Player_Inventory->Show(false);
				Player_Inventory->SetRenderItems(false);
			}
			ImGui::EndPopup();
		}
	}
}

void Game::OnRenderDebugGui() {
	if (ShowDebug) {
		ImGui::Begin("Debug Box");

		static float RenderPlotData[100] = { 0.0f };
		int RenderPlotIndex = 0;

		static float LogicPlotData[100] = { 0.0f };
		int LogicPlotIndex = 0;

		static float PlotData[100] = { 0.0f };
		int PlotIndex = 0;

		std::deque<float>& RenderFrameTimes = DebugUI::GetInstance("1")->getRenderFrameTimes();
		std::deque<float>& LogicFrameTimes = DebugUI::GetInstance("1")->getLogicFrameTimes();
		std::deque<float>& FrameTimes = DebugUI::GetInstance("1")->getFrameTimes();

		for (float time : RenderFrameTimes) {
			RenderPlotData[RenderPlotIndex++] = time;
		}
		for (float time : LogicFrameTimes) {
			LogicPlotData[LogicPlotIndex++] = time;
		}
		for (float time : FrameTimes) {
			PlotData[PlotIndex++] = time;
		}

		ImGui::PlotLines("Render Frame Time", RenderPlotData, RenderPlotIndex, 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));
		float sum = 0.0f;
		for (int i = 0; i < 10; i++) {
			int frameIndex = (RenderPlotIndex - i + 100) % 100;
			sum += RenderPlotData[frameIndex];
		}
		float average = sum / 10.0f;
		ImGui::Text("Latest Frame Time: %.3f ms", average);
		
		ImGui::PlotLines("Logic Frame Time", LogicPlotData, LogicPlotIndex, 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));
		sum = 0.0f;
		for (int i = 0; i < 10; i++) {
			int frameIndex = (LogicPlotIndex - i + 100) % 100;
			sum += LogicPlotData[frameIndex];
		}
		average = sum / 10.0f;
		ImGui::Text("Latest Frame Time: %.3f ms", average);

		ImGui::PlotLines("Tester Frame Time", PlotData, PlotIndex, 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));
		sum = 0.0f;
		for (int i = 0; i < 10; i++) {
			int frameIndex = (PlotIndex - i + 100) % 100;
			sum += PlotData[frameIndex];
		}
		average = sum / 10.0f;
		ImGui::Text("Latest Frame Time: %.3f ms", average);

		ImGui::Text("Global Object count:%i", Object::GetObjectCount());
		ImGui::Text("Player LIFE x:%i y:%i", player->GetLife(), 100);
		ImGui::Text("Player pos x:%f y:%f", player->GetPos().x, player->GetPos().y);
		ImGui::Text("Player vel x:%f y:%f", player->GetVel().x, player->GetVel().y);
		ImGui::Text("action:%d side:%d", player->GetCurrentAction(),player->GetCurrentSide());
		ImGui::Text("Camera pos x:%d y:%d w:%d h:%d.", Window::GetCamera()->X(),
			Window::GetCamera()->Y(),
			Window::GetCamera()->W(),
			Window::GetCamera()->H());
		ImGui::Text("Camera offset x:%f y:%f", Window::GetCamera()->GetOffset().x,
			Window::GetCamera()->GetOffset().y);
		ImGui::Text("Collision items count:%d", Collider::GetSize());
		ImGui::Text("player move coords:%f %f", player->GetMoveTarget().x, player->GetMoveTarget().y);
		Vec2 dif = player->GetPos() - Window::GetCamera()->GetPos() - player->GetMoveTarget();
		Vec2 ndif = dif.GetNormalized();
		ImGui::Text("player relative pos:%f %f", dif.x, dif.y);
		ImGui::Text("player normalized diff:%f %f", ndif.x, ndif.y);
		ImGui::Checkbox("Show Collisions borders", &DrawCollisions);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (GuiBox_MainMenu) {
			ImGui::Text("GuiboxPOS %f  %f", GuiBox_MainMenu->GetPos().x, GuiBox_MainMenu->GetPos().y);
			ImGui::Text("GuiboxSIZE %f  %f", GuiBox_MainMenu->GetSize().x, GuiBox_MainMenu->GetSize().y);
		}
		if (GuiButton_MainMenu_NewGame) {
			ImGui::Text("ButtonPOS %f  %f", GuiButton_MainMenu_NewGame->GetPos().x, GuiButton_MainMenu_NewGame->GetPos().y);
			ImGui::Text("ButtonGlobalPOS %f  %f", GuiButton_MainMenu_NewGame->GetGlobalPos().x, GuiButton_MainMenu_NewGame->GetGlobalPos().y);
			ImGui::Text("ButtonSize %f  %f", GuiButton_MainMenu_NewGame->GetSize().x, GuiButton_MainMenu_NewGame->GetSize().y);
		}
		ImGui::Text("Mouse Pos %f  %f", Mouse::GetPos().x, Mouse::GetPos().y);
		ImGui::Text("WindowSize %f  %f", Window::GetSize().x, Window::GetSize().y);
		ImGui::Text("CameraSize %f  %f", Window::GetCamera()->GetViewport().x, Window::GetCamera()->GetViewport().y);
		ImGui::Text("CameraPos %f  %f", Window::GetCamera()->GetPos().x, Window::GetCamera()->GetPos().y);

		ImGui::End();
	}
}

void Game::SaveGame() {
	StringBuffer ss;
	PrettyWriter<StringBuffer> writer(ss);

	std::string filename = "Data/Saves/" + savename;
	std::filesystem::path p;
	std::filesystem::create_directory(filename);

	player->Serialize(&writer);
	player->SerializeToFile(filename+"/Player.json");
	ss.Clear();
	writer.Reset(ss);

	Player_Inventory->Serialize(&writer);
	Player_Inventory->SerializeToFile(filename+"/Inventory.json");
	ss.Clear();
	writer.Reset(ss);

	writer.StartArray();
	for (int i = 0; i < Monster::MonsterList.size(); i++) {
		Monster::MonsterList[i]->Serialize(&writer);
	}
	writer.EndArray();
	std::ofstream fff(filename+"/Monsters.json");
	std::string sus = ss.GetString();
	fff << sus;
	fff.flush();
	fff.close();
	ss.Clear();
	writer.Reset(ss);

	writer.StartArray();
	for (int i = 0; i < Item::AllModifiers.size(); i++) {
		Item::AllModifiers[i]->Serialize(&writer);
	}
	writer.EndArray();

	std::ofstream f("Data/Modifiers.json");
	std::string s = ss.GetString();
	f << s;
	f.flush();
	f.close();

	ss.Clear();
	writer.Reset(ss);
	writer.StartArray();
	for (int i = 0; i < Monster::MonsterTypes.size(); i++) {
		Monster::MonsterTypes[i]->Serialize(&writer);
	}
	writer.EndArray();

	std::ofstream ff("Data/Monsters.json");
	std::string sa = ss.GetString();
	ff << sa;
	ff.flush();
	ff.close();

	if (_engine) {
		_engine->saves.clear();
		for (const auto& file : std::filesystem::directory_iterator("Data/Saves/")) {
			std::string savename;
			savename = file.path().filename().string();
			_engine->saves.push_back(savename);
			cout <<"Found save:" << file.path().filename().string() << endl;
		}
	}
}

void Game::LoadGame() {
	for (auto i : Player_Inventory->grid) {
		i->SetLocked(false);
		if (i->getHaveitem()) {
			i->GetItem()->render = false;
		}
	}
	for (auto i : Player_Inventory->equipment) {
		i->SetLocked(false);
		if (i->getHaveitem()) {
			i->GetItem()->render = false;
		}
	}
	player->SetPos(Vec2(300.0f, 300.0f));

	std::string filename = "Data/Saves/" + savename;
	player->DeserializeFromFile(filename+"/Player.json");
	Player_Inventory->DeserializeFromFile(filename+"/Inventory.json");

	std::ifstream f(filename+"/Monsters.json");
	std::stringstream buffer;
	buffer << f.rdbuf();
	f.close();

	rapidjson::Document doc;
	doc.Parse(buffer.str().c_str());
	
	rapidjson::Value& arr = doc;
	
	Monster::MonsterList.clear();
	for (rapidjson::SizeType i = 0; i < arr.Size(); i++) {
		const rapidjson::Value& c = arr[i];
		Monster* m = new Monster;
		m->Deserialize(c);
		m->SetParams(Monster::MonsterTypes[m->monsterType]);
		m->Deserialize(c);
		m->UpdateId();
		layer_player->Connect(m);
		Monster::MonsterList.push_back(m);
	}

	Engine::Pause = false;
	if (_engine) {
		_engine->deleted = true;
	}

	Player_Inventory->Show(false);
	GuiBox_StatsBar->Show(true);
	player->isDead = false;
	Inventory::inventoryChanged = true;
	Collider::RegisterObject(player, player->GetColliderPos(), player->GetColliderBox(), false);
	Collider::RegisterTarget(player, player->GetColliderPos(), player->GetColliderBox());
	ShowLoad = false;
	if (GuiBox_MainMenu) GuiBox_MainMenu->Show(false);
	ShowOptions = false;
	_gameState = PLAY;
}

void Game::NewGame(const std::string& saveName) {
	savename = saveName;
	Engine::Pause = false;
	ShowLoad = false;
	if (GuiBox_MainMenu) GuiBox_MainMenu->Show(false);
	ShowOptions = false;
	_gameState = PLAY;
	map2->SpawnMonsters();
	for (auto i : Monster::MonsterList) {
		layer_player->Connect(i);
	}
	player->SetPos(Vec2(300.0f, 300.0f));
	GuiBox_StatsBar->Show(true);
	player->isDead = false;
	Collider::RegisterObject(player,player->GetColliderPos(),player-> GetColliderBox(),false);
	Collider::RegisterTarget(player,player->GetColliderPos(),player-> GetColliderBox());
	if (_engine) {
		_engine->deleted = true;
	}
	for (auto i : Player_Inventory->grid) {
		i->SetLocked(false);
	}
}

// DrawOptionsMenu removed - functionality moved to OptionsScene
// DrawHelpMenu removed - functionality moved to HelpScene  
// DrawLoadMenu removed - functionality moved to LoadGameScene

void Game::OnGameCleanup() {
	Resources::UnloadAll();
	delete(map2);
}

void Game::RepositionGuiAtResChange() {
	// Reposition GUI elements when resolution changes
	// This can be implemented with specific GUI repositioning logic if needed
	if (GuiBox_StatsBar) {
		// Reposition stats bar if needed
	}
	if (Player_Inventory) {
		// Reposition inventory if needed
	}
}

// Old menu functions removed - functionality moved to scenes
// DrawOptionsMenu -> OptionsScene
// DrawHelpMenu -> HelpScene
// DrawLoadMenu -> LoadGameScene

void Game::SetupGame() {
	// Game setup logic if needed
}

void Game::CleanupGame() {
	// Game cleanup logic if needed
}

void Game::LoadImGuiSettings() {
	// Load ImGui settings from config file
	std::ifstream configFile("config.txt");
	if (configFile.is_open()) {
		std::string line;
		while (getline(configFile, line)) {
			// Remove whitespace
			line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return std::isspace(c); }), line.end());
			if (line.empty() || line[0] == '#') {
				continue;
			}
			
			auto delimiterPos = line.find("=");
			if (delimiterPos == std::string::npos) {
				continue;
			}
			
			auto name = line.substr(0, delimiterPos);
			auto value = line.substr(delimiterPos + 1);
			
			if (name == "master_volume") {
				int vol = std::stoi(value);
				Audio::SetMasterVolume(vol);
			} else if (name == "music_volume") {
				int vol = std::stoi(value);
				Audio::SetMusicVolume(vol);
			} else if (name == "fullscreen" && _engine) {
				_engine->check_fullscreen = (value == "true");
			}
		}
		configFile.close();
	}
}

void Game::SaveImGuiSettings() {
	// Save ImGui settings to config file
	// Read existing config first
	std::vector<std::string> lines;
	std::ifstream configIn("config.txt");
	if (configIn.is_open()) {
		std::string line;
		while (getline(configIn, line)) {
			// Skip existing ImGui settings
			if (line.find("master_volume") == std::string::npos &&
				line.find("music_volume") == std::string::npos &&
				line.find("fullscreen") == std::string::npos) {
				lines.push_back(line);
			}
		}
		configIn.close();
	}
	
	// Write back with updated settings
	std::ofstream configOut("config.txt");
	if (configOut.is_open()) {
		for (const auto& line : lines) {
			configOut << line << "\n";
		}
		configOut << "# ImGui Settings\n";
		configOut << "master_volume=" << Audio::GetMasterVolume() << "\n";
		configOut << "music_volume=" << Audio::GetMusicVolume() << "\n";
		if (_engine) {
			configOut << "fullscreen=" << (_engine->check_fullscreen ? "true" : "false") << "\n";
		}
		configOut.close();
	}
}
