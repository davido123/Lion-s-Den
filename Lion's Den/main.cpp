/*!
 boldtemple Gaming ©, http://gaming.boldtemple.net
 OpenSource Project - Check Development, License & Readme Sections.

 BGE - boldtemple Cross Platform Game Engine
 /main.cpp
 !*/
#include "Engine.h"
#include <iostream>
#include <string>

#include "Core/Vec2.h"
#include "Core/Resources.h"

#include "GUI/Text.h"
#include "GUI/Widgets/Box.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/StatsBar.h"
#include "GUI/Widgets/Inventory.h"
#include "GUI/Widgets/CharacterStats.h"

#include "GUI/Window.h"

#include "Common/FPScounter.h"
#include "Render/Drawer.h"
#include "Player.h"
#include "Map.h"


#include "ImGUI/imgui.h"
#include "ImGUI/imgui_sdl.h"
#include "ImGui/imgui_struct.h"

#include "Core/Collider.h"
#include <Monster.h>
#include <Settings.h>
#include <DebugUI.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <io/JsonBase.h>
#include <chrono>
#include <iomanip>

#undef main

#ifndef BUILD_STATIC_LIB
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

//#Globals
	//#Layers
	Object* layer_gui;
	Object* layer_player;

//#Gui Elements
	//##Box(gui containers)
	Box* GuiBox_MainMenu;
	Box* GuiBox_MenuOptions;
	StatsBar* GuiBox_StatsBar;
	Inventory* Player_Inventory;
	
	//
	Box* GuiBox_SaveMenu;
	Box* GuiBox_SaveMenu_Upper;
	Box* GuiBox_SaveMenu_Lower;

	//##Buttons
	Button* GuiButton_MainMenu_Start;
	Button* GuiButton_MainMenu_Options;
	Button* GuiButton_MainMenu_ExitGame;
	Button* GuiButton_MainMenu_NewGame;
	Button* GuiButton_MainMenu_LoadGame;
	//
	Button* GuiButton_Save_Slot1;
	Button* GuiButton_Save_Slot2;
	Button* GuiButton_Save_Slot3;
	Button* GuiButton_PlayNewGame;
	Button* GuiButton_LoadGame;
	Button* GuiButton_SaveGame;



//#Elements
	Player* player;
	Monster* monster1;
	Monster* monster2;
	Monster* monster3;
	Map* map2;


	//singletons
	Settings* settings;
	DebugUI* debugui;


//#Debug Gui bool
	bool ShowDebug = 0;
	bool DrawCollisions = 0;
	bool ShowOptions = 0;
	bool ShowLoad = 0;
	bool ShowHelp = 0;

	bool Engine::Pause=0;

//#Game State
	int _gameState=MAIN_MENU;
	std::string savename;

//#####################################################End Of Globals
void Engine::OnInit() {
	Item::FillModifiersFile();
	saves.clear();
	for (const auto& file : std::filesystem::directory_iterator("Data/Saves/"))
	{
		
		std::string savename;
		savename = file.path().filename().string();
		
		saves.push_back(savename);
		cout <<"Found save:" << file.path().filename().string() << endl;
	}
	Audio *a = new Audio("Data/Sounds/test.wav",true);
	a->Play(1);
	
	




	settings = Settings::GetInstance("1");
	debugui = DebugUI::GetInstance("1");
	//settings->Load_File("Settings.json");

	
	//Load map file
	map2 = new Map;
	map2->LoadJson("Data/Maps/Map.json");
	map2->GetCollisionBoxes();
	map2->RegisterCollisionBoxes();
	Monster::SaveMonsterTypes();

	//Initializing layers
	Engine::AddLayer();
	Engine::AddLayer();
	Engine::AddLayer();
	layer_player = Engine::GetRootAtLayer(0);
	layer_player->_custom_render = true;
	layer_gui = Engine::GetRootAtLayer(1);

	player = new Player();
	Collider::RegisterObject(player,player->GetColliderPos(),player-> GetColliderBox(),false);
	Collider::RegisterTarget(player,player->GetColliderPos(),player-> GetColliderBox());


	//Initializing Gui elements
	//Calculate pos and size of menu box

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

	/************Player Hud******************/
	GuiBox_StatsBar			= new StatsBar(Vec2(0,0),Vec2(300,64));
	GuiBox_StatsBar->SetStyle("GUI/hud.png");
	layer_gui->Connect(GuiBox_StatsBar);
	GuiBox_StatsBar->Show(false);
	

	/***************Inventory****************/

	//MainMenuSize

	Player_Inventory = new Inventory(Vec2(80,40),Vec2(35,60), Resources::GetDefaultFont(),16);
	Player_Inventory->Show(false);
	layer_gui->Connect(Player_Inventory);
	GuiBox_StatsBar->ConnectPlayer(player);

	Player_Inventory->ConnectPlayer(player);
	layer_player->Connect(player);
}

void Engine::OnEvent(SDL_Event *event, const Uint8 *keyboardState) {
	
	
	if (event->type == SDL_KEYDOWN) {
		if (Keyboard::isKeyDown(KEY_ESCAPE)) {
			Engine::Pause = true;
			

			
		}
		if (Keyboard::isKeyDown(KEY_GRAVE)) {
			ShowDebug = !ShowDebug;
			//GuiBox_StatsBar->SetRenderOption(!GuiBox_StatsBar->GetRenderOption());
		}

	}
	if (_gameState == MAIN_MENU)
	{
		if (Mouse::InWindow()) {
			if (!ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse)
			{
				if (Mouse::Pressed(MOUSE_LEFT)) {
					if (GUI::GetLastClicked() != nullptr)
						if (GUI::GetLastClicked()->GetLabel() == "Quit Game")
						{
							Engine::Stop();
						}
					
						else if (GUI::GetLastClicked()->GetLabel() == "Options")
						{
							GuiBox_MainMenu->Show(false);
							ShowOptions = true;

						}
						else if (GUI::GetLastClicked()->GetLabel() == "Play")
						{
							ShowLoad = true;

							GuiBox_MainMenu->Show(false);


						}
						else if (GUI::GetLastClicked()->GetLabel() == "Help")
						{
							ShowHelp = true;

							GuiBox_MainMenu->Show(false);


						}
				}
			}
		}
	}
	if (_gameState == PLAY)
	{
		if (event->type == SDL_KEYDOWN) {
			if (Keyboard::isKeyDown(KEY_I)) {
				Player_Inventory->Show(!Player_Inventory->IsVisible());
				Player_Inventory->SetRenderItems(Player_Inventory->IsVisible());
			}
			if (Keyboard::isKeyDown(KEY_C)) {
				player->drawStatBox = !player->drawStatBox;

			}
			

		}
		if (event->type == SDL_MOUSEBUTTONDOWN)
		{
			Player_Inventory->_mbtpressed = true;
			Player_Inventory->_mbtreleased = false;

		}
		if (event->type == SDL_MOUSEBUTTONUP)
		{
			Player_Inventory->_mbtpressed = false;
			Player_Inventory->_mbtreleased = true;
		}

	}
	
}
void Engine::OnUpdate() {
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;

	if (_gameState == PLAY)
	{
		for (auto i : Monster::MonsterList)
		{
			i->SetMoveTarget(player->GetPos());
		}

		Window::GetCamera()->CenterOnPlayer(player->GetPos(), player->GetSize(), map2->GetMapSizePixels());
	}
}
void Engine::OnRender() {
	ImGui::ShowDemoWindow();
	if (_gameState == PLAY)
	{
		map2->DrawLayer("Floor");
		map2->DrawLayer("Floor2");
		map2->DrawLayerOrderNR("Walls", 0);
		_Layers[0]->RenderChildren();
		map2->DrawLayerOrderNR("Walls", 1);

		if (DrawCollisions)
			Collider::DrawCollisions();



		if (Engine::Pause)
		{
			ImGui::OpenPopup("Pause");
			if (ImGui::BeginPopupModal("Pause", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Do you want to exit and save game?");
				ImGui::Separator();
				if (ImGui::Button("Yes", ImVec2(120, 0))) {

					SaveGame();
					Engine::Pause = false;
					_gameState = MAIN_MENU;
					GuiBox_MainMenu->Show(true);

					GuiBox_StatsBar->Show(false);
					Player_Inventory->Show(false);
					Player_Inventory->Clear();
					Player_Inventory->overlay->DisconnectAll();



					for (auto i : Monster::MonsterList)
					{
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
		if (player->isDead)
		{
			ImGui::OpenPopup("Dead");
			if (ImGui::BeginPopupModal("Dead", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("You are dead, exit to menu?");
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();
				if (ImGui::Button("Yes", ImVec2(120, 0))) {

					//SaveGame();
					Engine::Pause = false;
					_gameState = MAIN_MENU;
					GuiBox_MainMenu->Show(true);

					GuiBox_StatsBar->Show(false);
					Player_Inventory->Show(false);
					Player_Inventory->Clear();
					Player_Inventory->overlay->DisconnectAll();



					for (auto i : Monster::MonsterList)
					{
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

	_Layers[1]->RenderChildren();

	Drawer::RenderAll(Window::GetRenderer(), Window::GetCamera());


}

void DebugUI::RenderDebug()
{
	if (ShowDebug)// #DEBUGBOX
	{
		ImGui::Begin("Debug Box");

		
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

		ImGui::Text("GuiboxPOS %f  %f", GuiBox_MainMenu->GetPos().x, GuiBox_MainMenu->GetPos().y);
		ImGui::Text("GuiboxSIZE %f  %f", GuiBox_MainMenu->GetSize().x, GuiBox_MainMenu->GetSize().y);
		ImGui::Text("ButtonPOS %f  %f", GuiButton_MainMenu_NewGame->GetPos().x, GuiButton_MainMenu_NewGame->GetPos().y);
		ImGui::Text("ButtonGlobalPOS %f  %f", GuiButton_MainMenu_NewGame->GetGlobalPos().x, GuiButton_MainMenu_NewGame->GetGlobalPos().y);
		ImGui::Text("ButtonSize %f  %f", GuiButton_MainMenu_NewGame->GetSize().x, GuiButton_MainMenu_NewGame->GetSize().y);



		ImGui::Text("Mouse Pos %f  %f", Mouse::GetPos().x, Mouse::GetPos().y);
		ImGui::Text("WindowSize %f  %f", Window::GetSize().x, Window::GetSize().y);
		ImGui::Text("CameraSize %f  %f", Window::GetCamera()->GetViewport().x, Window::GetCamera()->GetViewport().y);
		ImGui::Text("CameraPos %f  %f", Window::GetCamera()->GetPos().x, Window::GetCamera()->GetPos().y);

		//	



		ImGui::End();
		//ImGui::ShowDemoWindow();

	}
 
}

void Engine::OnRenderDebugGui() {
	
	debugui->RenderDebug();
}

void Engine::SaveGame() {


	StringBuffer ss;

	PrettyWriter<StringBuffer> writer(ss);
	//PrettyWriter<StringBuffer> writer2(ss);

	std::string filename = "Data/Saves/" +savename;
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
	for (int i = 0; i < Monster::MonsterList.size(); i++)
    {
        Monster::MonsterList[i]->Serialize(&writer);
	}
	writer.EndArray();
	std::ofstream fff(filename+"/Monsters.json");
	std::string sus =ss.GetString();
	fff << sus;
	fff.flush();
	fff.close();
	ss.Clear();
	writer.Reset(ss);
	




	writer.StartArray();
	for (int i = 0; i < Item::AllModifiers.size(); i++)
    {
        Item::AllModifiers[i]->Serialize(&writer);
	}
	writer.EndArray();

	std::ofstream f("Data/Modifiers.json");
	std::string s =ss.GetString();
	f << s;
	f.flush();
	f.close();

	ss.Clear();
	writer.Reset(ss);
	writer.StartArray();
	for (int i = 0; i < Monster::MonsterTypes.size(); i++)
	{
		Monster::MonsterTypes[i]->Serialize(&writer);

	}
	



	writer.EndArray();
	std::ofstream ff("Data/Monsters.json");
	std::string sa =ss.GetString();
	ff << sa;
	ff.flush();
	ff.close();

	saves.clear();
	for (const auto& file : std::filesystem::directory_iterator("Data/Saves/"))
	{
		
		std::string savename;
		savename = file.path().filename().string();
		
		saves.push_back(savename);
		cout <<"Found save:" << file.path().filename().string() << endl;
	}
	



}
void Engine::LoadGame() {
	for (auto i : Player_Inventory->grid)
	{
		i->SetLocked(false);
		if (i->getHaveitem())
		{
			i->GetItem()->render = false;
		}
	}
	for (auto i : Player_Inventory->equipment)
	{
		i->SetLocked(false);
		if (i->getHaveitem())
		{
			i->GetItem()->render = false;
		}
	}
	player->SetPos(Vec2(300.0f, 300.0f));

	//Fill Modifiers vector from code
	
	std::string filename = "Data/Saves/" +savename;
	player->DeserializeFromFile(filename+"/Player.json");
	Player_Inventory->DeserializeFromFile(filename+"/Inventory.json");

	std::ifstream f(filename+"/Monsters.json");
	std::stringstream buffer;
	buffer << f.rdbuf();
	f.close();

	rapidjson::Document doc;
	doc.Parse(buffer.str().c_str());
	
	rapidjson::Value& arr= doc;
	
	Monster::MonsterList.clear();
    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
        const rapidjson::Value& c = arr[i];		

		Monster* m = new Monster;
		
		m->Deserialize(c);// deserialize to get monster type
		m->SetParams(Monster::MonsterTypes[m->monsterType]);//set params of monster type
		m->Deserialize(c);// deserialize again to load saved params
		m->UpdateId();
		layer_player->Connect(m);

		Monster::MonsterList.push_back(m);
    }

	Engine::Pause = false;
	deleted = true;//for reloading savelist

	Player_Inventory->Show(false);
	GuiBox_StatsBar->Show(true);
	player->isDead = false;
	Inventory::inventoryChanged = true;
	Collider::RegisterObject(player, player->GetColliderPos(), player->GetColliderBox(), false);
	Collider::RegisterTarget(player, player->GetColliderPos(), player->GetColliderBox());
	ShowLoad = false;
	GuiBox_MainMenu->Show(false);
	ShowOptions = false;
	_gameState = PLAY;






}



void Engine::DrawOptionsMenu(){
	if(ShowOptions)
	{ 
		
		Vec2 MainWindowSize = Window::GetSize();
		Vec2 WindowPos = MainWindowSize/4;
		Vec2 WindowSize= MainWindowSize/2;

		ImVec2 ImWindowPos=ImVec2(WindowPos.x,WindowPos.y);
		ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);;

		ImGui::SetNextWindowPos(ImWindowPos);
		ImGui::SetNextWindowSize(ImWindowSize);

        // #Options
		ImGui::Begin("Opcje");
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Graphics"))
			{
				int display_index=0, display_count=0;
				int modes= SDL_GetNumDisplayModes(0);
				vector <SDL_DisplayMode*> SDL_DisplayModes;
				for (int i = 0; i < modes; i++){
					SDL_DisplayMode* mode = new SDL_DisplayMode{ SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
					
					if (SDL_GetDisplayMode(display_index, i, mode) != 0) {
						SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
					}
					SDL_DisplayModes.push_back(mode);
				}

				auto values = ""s;
				for (int i = 0; i < SDL_DisplayModes.size(); i++){
					string test = int_to_str(SDL_DisplayModes[i]->w) + " x " + int_to_str(SDL_DisplayModes[i]->h) + " : " + int_to_str(SDL_DisplayModes[i]->refresh_rate);
					values += test +"\000"s;
				}

				static int item_current_2 = 0;
				ImGui::Combo("Resolution", &item_current_2, values.c_str());
				ImGui::Checkbox("Fullscreen", &check_fullscreen);

				if (ImGui::Button("Apply"))
				{
					
					SDL_SetWindowDisplayMode(Window::GetWindow(), SDL_DisplayModes.at(item_current_2));
					if (check_fullscreen)
					{
						//SDL_SetWindowFullscreen(Window::GetWindow(), SDL_WINDOW_FULLSCREEN);
						Window::SetMode(SDL_DisplayModes.at(item_current_2)->w, SDL_DisplayModes.at(item_current_2)->h, check_fullscreen,"Lion's Den");
						Surface::BeginViewport(Vec2::ZERO, Window::GetCamera()->GetViewport());
						//RepositionGuiAtResChange();
						reschange = true;
					}
					else if(!check_fullscreen)
					{
						//SDL_SetWindowFullscreen(Window::GetWindow(), 0);
						Window::SetMode(SDL_DisplayModes.at(item_current_2)->w, SDL_DisplayModes.at(item_current_2)->h, check_fullscreen, "Lion's Den");
						Surface::BeginViewport(Vec2::ZERO, Window::GetCamera()->GetViewport());
						//RepositionGuiAtResChange();
						//RepositionGuiAtResChange();
						//SDL_SetWindowSize(Window::GetWindow(), SDL_DisplayModes.at(item_current_2)->w,SDL_DisplayModes.at(item_current_2)->h);
						reschange = true;

					}
				}
				ImGui::SameLine();
				ImGui::SameLine();
				if (ImGui::Button("Back"))
				{
					ShowOptions = false;
					GuiBox_MainMenu->Show(true);
				}
				ImGui::EndTabItem();
				for (int i = 0; i < SDL_DisplayModes.size(); i++){
					delete SDL_DisplayModes[i];
				}
			}
			if (ImGui::BeginTabItem("Inne")){
				ImGui::Text("Dzial w budowie");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}
void Engine::DrawHelpMenu()
{
	if(ShowHelp)
	{ 
		Vec2 MainWindowSize = Window::GetSize();
		Vec2 WindowPos = MainWindowSize/4;
		Vec2 WindowSize= MainWindowSize/2;

		ImVec2 ImWindowPos=ImVec2(WindowPos.x,WindowPos.y);
		ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);;

		ImGui::SetNextWindowPos(ImWindowPos);
		ImGui::SetNextWindowSize(ImWindowSize);
		

		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;

		static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
		

		ImGui::Begin("Help",NULL,window_flags);

		ImGui::BeginTabBar("Bar");
		if (ImGui::BeginTabItem("Intro"))
		{
			ImGui::BeginChild("child", ImVec2(0,ImWindowSize.y*0.7), true);
			ImGui::TextWrapped("Witaj w grze Lion's Den\n\nLion's Den jest gra hack&slash z elementami RPG\nTa gra jest projektem stworzonym w celu doskonalenia wlasnych umiejetnosci programistycznych\nGra powstala glownie przy uzyciu jezyka C++ oraz Biblioteki SDL2\nAby dowiedziec sie jak grac sprawdz powyzsze zakladki");
			
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Sterowanie"))
		{
			ImGui::BeginChild("child", ImVec2(0,ImWindowSize.y*0.7), true);
			ImGui::TextWrapped("Poruszanie sie: Klawisze AWSD \nAtakowanie: PPM\nBieganie: LShift\nEkwipunek: I\nStatystyki postaci: C");




			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mechaniki Gry"))
		{
			ImGui::BeginChild("child", ImVec2(0,ImWindowSize.y*0.7), true);

			ImGui::TextWrapped("Rozwoj postaci polega na zdobywaniu przedmiotow z pokonanych przeciwnikow\nPrzedmioty jakie mozemy spotkac w grze to:\nHelmy, Zbroje, Rekawiczki, Buty, Pierscienie, Amulety, Pasy, Miecze oraz Tarcze\n");
			ImGui::TextWrapped("Kazdy z wymienionych przedmiotow jest klasyfikowany poprzez rzadkosc:\nNormalny, Magiczny, Rzadki, Unikatowy\nod rzadkosci przedmiotu zalezy to ile modyfikatorow statystyk dany przedmiot bedzie posiadal np: Normalny-0, Unikatowy-4");
			ImGui::TextWrapped("Po najechaniu kursorem myszki na przedmiot wyswietla sie jego statystyki, ponizej widoczne sa przykladowe przedmioty losowo wygenerowane");







			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Credits"))
		{
			ImGui::BeginChild("child", ImVec2(0,ImWindowSize.y*0.7), true);
			ImGui::TextWrapped("Created by Dawid Dziegielewski");




			ImGui::EndChild();
			ImGui::EndTabItem();
		}
			

		
		ImGui::EndTabBar();

		ImGui::Separator();
		if (ImGui::Button("Back"))
		{
			ShowHelp = false;
			GuiBox_MainMenu->Show(true);


		}



		
		ImGui::End();
	}
}
void Engine::DrawLoadMenu()
{
	if(ShowLoad)
	{ 
		Vec2 MainWindowSize = Window::GetSize();
		Vec2 WindowPos = MainWindowSize/4;
		Vec2 WindowSize= MainWindowSize/2;

		ImVec2 ImWindowPos=ImVec2(WindowPos.x,WindowPos.y);
		ImVec2 ImWindowSize = ImVec2(WindowSize.x, WindowSize.y);;

		ImGui::SetNextWindowPos(ImWindowPos);
		ImGui::SetNextWindowSize(ImWindowSize);
		


		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;

        // #LoadMenu
		static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
		if (deleted)
		{

			saves.clear();
			for (const auto& file : std::filesystem::directory_iterator("Data/Saves/"))
			{
		
				std::string savename;
				savename = file.path().filename().string();
		
				saves.push_back(savename);
	
			}
			deleted = false;
		}
		

		ImGui::Begin("Load Game",NULL,window_flags);


		
		if (ImGui::BeginTable("Saves", 4,flags))
        {
			ImGui::TableSetupColumn("NR");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Time");
			ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

			int rows=saves.size();

            for (int row = 0; row < rows; row++)
            {
                ImGui::TableNextRow();


                for (int column = 0; column < 4; column++)
                {
					if (column == 0)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("%d", row+1);
					}
					if (column == 1)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("%s",saves.at(row).c_str());
					}
					if (column == 2)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("Time");
					}
					if (column == 3)
					{
						ImGui::TableSetColumnIndex(column);
						
						ImGui::PushID(row);
						if (ImGui::Button("Load Game"))
						{
							savename = saves.at(row);
							LoadGame();
							
							LoadedSaveNr = row;
						}
						ImGui::PopID();
						ImGui::SameLine();
						ImGui::PushID(row);
						if (ImGui::Button("Delete"))
						{
							savename = saves.at(row);
							std::filesystem::remove_all("Data/Saves/"+savename);
							deleted = true;//for reloading savelist
						}
						ImGui::PopID();
						
					}

                }
            }
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d",rows+1);
			ImGui::TableSetColumnIndex(1);
            ImGui::Text("Empty Slot");
			ImGui::TableSetColumnIndex(3);
			if (ImGui::Button("New Game"))
			{
				LoadedSaveNr = rows + 1;
				ImGui::OpenPopup("New Game");
			}
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("New Game", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Please specify a name for a save file");
				ImGui::Separator();

				static char name[32] = "save";
				ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));

				ImGui::Separator();
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					Engine::Pause = false;
					ShowLoad = false;
					GuiBox_MainMenu->Show(false);
					ShowOptions = false;
					_gameState = PLAY;
					map2->SpawnMonsters();
					for (auto i : Monster::MonsterList)
					{
						layer_player->Connect(i);
					}

					player->SetPos(Vec2(300.0f, 300.0f));
					GuiBox_StatsBar->Show(true);
					savename = name;
					player->isDead = false;
					Collider::RegisterObject(player,player->GetColliderPos(),player-> GetColliderBox(),false);
					Collider::RegisterTarget(player,player->GetColliderPos(),player-> GetColliderBox());
					deleted = true;//for reloading savelist
					ImGui::CloseCurrentPopup(); 
					for (auto i : Player_Inventory->grid)
					{
						i->SetLocked(false);
					}
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}
            ImGui::EndTable();
			if (ImGui::Button("Back"))
			{
				ShowLoad = false;
				GuiBox_MainMenu->Show(true);
			}
        }
			
		ImGui::End();
	}
}


void Engine::RepositionGuiAtResChange()
{
	ImGuiSDL::Deinitialize();
	ImGuiSDL::Initialize(Window::GetRenderer(), Window::GetWidth(), Window::GetHeight());

	GuiBox_MainMenu->RecalcRelative();
	GuiBox_MainMenu->RecalcCentered();
	Player_Inventory->RecalcRelative();
	GuiBox_StatsBar->RecalcRelative();
}


void Engine::OnCleanUp() {
	Resources::UnloadAll();
	delete(map2);
}
int main() {
	Engine engine;
	engine.Start();
	return 0;
}
#endif
