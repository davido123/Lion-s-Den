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

#include "GUI/Window.h"

#include "Common/FPScounter.h"
#include "Render/Drawer.h"
#include "Player.h"
#include "Map.h"

#undef main

#ifndef BUILD_STATIC_LIB

//###Globals
//##Layers
Object* layer_ground;
Object* layer_objects;
Object* layer_player;
Object* layer_gui;

//#Gui Elements

Box* mainMenu;
Box* menuOptions;
Box* hud;
Button* Button_mainMenu_start;
Button* Button_mainMenu_options;
Button* Button_mainMenu_exit;


//#elements
Text* test;
Player* player;
Box* a;
Map* map;
std::string stringtest = "test:";


//////////////////testy
double angle = 0;
Vec2 pos;
Vec2 center;
int ii = 0;
bool GameState_play = false;



void Engine::OnInit() {

	map = new Map;
	map->Load("Data/Maps/Map.txt");
	



	//Initializing layers
	Engine::AddLayer();
	Engine::AddLayer();
	Engine::AddLayer();
	Engine::AddLayer();
	layer_ground = Engine::GetRootAtLayer(0);
	layer_objects = Engine::GetRootAtLayer(1);
	layer_player = Engine::GetRootAtLayer(2);
	layer_gui = Engine::GetRootAtLayer(3);

	//Initializing Gui elements
	//Calculate pos and size of menu box
	Vec2 menusize = Window::GetSize()/2.0f;
	
	mainMenu = new Box(menusize/2, menusize, Resources::GetDefaultFont(), 16);
	menuOptions = new Box(menusize / 2, menusize, Resources::GetDefaultFont(), 16);
	hud = new Box(Vec2(0.0f,0.0f), Vec2(200.0f, 70.0f), Resources::GetDefaultFont(), 16);


	layer_gui->Connect(hud);
	menuOptions->Show(false);
	layer_gui->Connect(mainMenu);
	layer_gui->Connect(menuOptions);
	//layer_gui->Connect(gui);
	

	Button_mainMenu_start = new Button(Vec2(mainMenu->GetPos().x/2, 50.0f), Vec2(70.0f, 50.0f), Resources::GetDefaultFont(), 16,"Start");
	Button_mainMenu_start->SetLabel("Start");
	mainMenu->Connect(Button_mainMenu_start);

	Button_mainMenu_options = new Button(Vec2(mainMenu->GetPos().x / 2, 150.0f), Vec2(100.0f, 50.0f), Resources::GetDefaultFont(), 16,"Options");
	Button_mainMenu_options->SetLabel("Options");
	mainMenu->Connect(Button_mainMenu_options);

	Button_mainMenu_exit = new Button(Vec2(mainMenu->GetPos().x / 2, 230.0f), Vec2(60.0f, 50.0f), Resources::GetDefaultFont(), 16,"Exit");
	Button_mainMenu_exit->SetLabel("Exit");
	mainMenu->Connect(Button_mainMenu_exit);

	
	//Plain text
	//test = new Text();
	//test->Init(600, 400, "GRASS", Resources::GetDefaultFont(), 14);















	player = new Player();
	player->SetPos(Vec2(300.0f,300.0f));
	layer_player->Connect(player);

	
	

}

void Engine::OnEvent(SDL_Event *event, const Uint8 *keyboardState) {
	if (event->type == SDL_KEYDOWN) {
		if (Keyboard::isKeyDown(KEY_ESCAPE)) {
			Engine::Stop();
		}
	}
	if (Mouse::InWindow()) {
		if (Mouse::Pressed(MOUSE_LEFT)) {
			if(GUI::GetLastClicked()!=nullptr)
				if(GUI::GetLastClicked()->GetLabel() == "Exit")
				{
					Engine::Stop();
				}
				else if (GUI::GetLastClicked()->GetLabel() == "Options")
				{
					mainMenu->Show(false);
					Button_mainMenu_start->Show(false);
					Button_mainMenu_options->Show(false);
					Button_mainMenu_exit->Show(false);
					menuOptions->Show(true);
				}
				else if (GUI::GetLastClicked()->GetLabel() == "Start")
				{
					mainMenu->Show(false);
					Button_mainMenu_start->Show(false);
					Button_mainMenu_options->Show(false);
					Button_mainMenu_exit->Show(false);
					GameState_play=true;
				}



			//std::cout << GUI::GetLastClicked()->GetLabel() << std::endl;
		}

	}


	


}


void Engine::OnUpdate() {
	//test->SetText(std::to_string(ii));

	mainMenu->CheckTop();
	Button_mainMenu_exit->CheckTop();

}

void Engine::OnRender() {
	//test->Draw();


	/*
	Vec2 screen_mouse_pos = Window::GetCamera()->GetPos() + Mouse::GetPos();


	Drawer::Line(Vec2(0, 0), screen_mouse_pos, COLOR_YELLOW);
	Drawer::Line(Vec2(Window::GetWidth(), 0), screen_mouse_pos, COLOR_YELLOW);
	Drawer::Line(Vec2(0, Window::GetHeight()), screen_mouse_pos, COLOR_YELLOW);
	Drawer::Line(Vec2(Window::GetWidth(), Window::GetHeight()), screen_mouse_pos, COLOR_YELLOW);

	Drawer::Rect(pos, Vec2(30, 30), COLOR_PURPLE, true, angle);
	Drawer::Line(pos + Vec2(15, 15), screen_mouse_pos, COLOR_MAGENTA);

	Drawer::Circle(screen_mouse_pos, screen_mouse_pos.GetLength() / 3, COLOR_RED, true, (center - screen_mouse_pos).GetAngle());

	Drawer::RenderAll(Window::GetRenderer(), Window::GetCamera());



	Window::SetBackgroundColor(Window::GetBackgroundColor());
	*/



	map->Draw();



}

void Engine::OnCleanUp() {
	Resources::UnloadAll();
	delete(map);
}
int main() {
	Engine engine;
	engine.Start();
	return 0;
}
#endif
//testing for github