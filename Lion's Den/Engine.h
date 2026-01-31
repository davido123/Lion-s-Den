/*!
  boldtemple Gaming ©, http://gaming.boldtemple.net
  OpenSource Project - Check Development, License & Readme Sections.

  BGE - boldtemple Cross Platform Game Engine
  /Engine.h
!*/

#ifndef ENGINE_H_
#define ENGINE_H_

#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#ifdef USE_PHYSFS
#include <physfs.hpp>
#endif

#include "Common/constants.h"

#include "GUI/GUI.h"
#include "GUI/Window.h"
#include "GUI/Cursor.h"

#include "Core/Object.h"
#include "Core/Resources.h"
#include "Core/Collider.h"

#include "Render/Camera.h"
#include "Render/Surface.h"

#include "IO/Mouse.h"
#include "IO/Keyboard.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_sdl.h"
#include "ImGui/imgui_impl_sdl.h"
#include <Render/Sprite.h>
#include <DebugUI.h>

class Engine {
public:
	Engine();
	virtual ~Engine();
	int GetGameSpeed();
	Object* GetRootAtLayer(unsigned int layer);
	
	void AddLayer();
	void SetGameSpeed(int ms);
	void SetVideo(int w, int h, bool full_screen, std::string win_title);
	void Start();
	void Stop();


private:
	std::vector<Object*> _Layers;
	std::vector<Object*> _DeleteCandidates;

	double _ms_per_update;

	void DeleteObjects();

	bool Core_Init();
	void Core_Event(SDL_Event* event, const Uint8* keyboardState);
	void Core_Update();
	void Core_Render();
	void Core_CleanUp();

	virtual void OnInit();
	virtual void OnEvent(SDL_Event* event, const Uint8* keyboardState);
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnCleanUp();

public://own functions
	bool check_fullscreen = false;
	bool quit;
	int LoadedSaveNr;
	Sprite Background;
	static bool Pause;
	std::vector<std::string> saves;

	bool deleted=true;
	bool reschange = false;


private:
	virtual void RepositionGuiAtResChange();
	virtual void OnRenderDebugGui();
	virtual void SaveGame();
	virtual void LoadGame();
};

#endif /* ENGINE_H_ */
