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

#include "Common/FPScounter.h"
#include "Render/Drawer.h"
#include "Player.h"

#undef main

#ifndef BUILD_STATIC_LIB


Object* layer_ground;
Object* layer_objects;
Object* layer_player;
Text* test;

Player* player;
Box* a;
//std::vector < Player* > players;
std::string stringtest = "test:";


//////////////////testy
double angle = 0;
Vec2 pos;
Vec2 center;
void Engine::OnInit() {

	Resources::SetDefaultFont("atwriter.ttf");




	Window::SetMode(800, 600, false, "Lion's Den");



	a = new Box(Vec2(100.0f, 100.0f), Vec2(200.0f, 50.0f), Resources::GetDefaultFont(), 16);




	a->SetBackGround("button.png", Vec2(0.0f, 0.0f), 32);
	a->ShowBack(1);
	//a-
	//a->set

	//a->SetIcon();
	//a->SetBackGround();
	//a->SetLabel();

	a->SetText("tt");






	test = new Text();
	test->Init(10, 10, "GRASS", Resources::GetDefaultFont(), 14);

	Engine::AddLayer();
	Engine::AddLayer();
	Engine::AddLayer();
	Engine::AddLayer();

	layer_ground = Engine::GetRootAtLayer(0);
	layer_objects = Engine::GetRootAtLayer(1);
	layer_player = Engine::GetRootAtLayer(2);

	player = new Player();
	layer_player->Connect(player);

	layer_objects->Connect(a);













	/*
	for(int i=0;i<=100;i++)
	{
		players.push_back(new Player());
		//std::cout<<"current player created"<<i<<std::endl;
	}


	for(int i=0;i<=100;i++)
	{
		layer_player->Connect(players.at(i));
		//std::cout<<"current player connected"<<i<<std::endl;
	}
*/



}

void Engine::OnEvent(SDL_Event *event, const Uint8 *keyboardState) {

	if (event->type == SDL_KEYDOWN) {
		if (Keyboard::isKeyDown(KEY_A)) {
			stringtest += "a";
		}
		if (Keyboard::isKeyDown(KEY_RETURN)) {
			a->SetText(stringtest);
		}

		if (Keyboard::isKeyDown(KEY_ESCAPE)) {
			Engine::Stop();
		}
	}




}

void Engine::OnUpdate() {
	test->SetText(std::to_string(GetGameSpeed()));

}

void Engine::OnRender() {
	test->Draw();



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






}

void Engine::OnCleanUp() {
	Resources::UnloadAll();
}

int main() {
	Engine engine;
	engine.Start();
	return 0;
}
#endif
