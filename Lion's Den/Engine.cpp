/*!
 boldtemple Gaming ©, http://gaming.boldtemple.net
 OpenSource Project - Check Development, License & Readme Sections.

 BGE - boldtemple Cross Platform Game Engine
 /Engine.cpp
 !*/
#include "Engine.h"

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void    Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (clear)
			Clear();
		if (copy)
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char* buf = Buf.begin();
		const char* buf_end = Buf.end();
		if (Filter.IsActive())
		{
			for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				if (Filter.PassFilter(line_start, line_end))
					ImGui::TextUnformatted(line_start, line_end);
			}
		}
		else
		{
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
};

Engine::Engine() {
}

Engine::~Engine() {
}

void SetVideo(int w, int h, bool full_screen, std::string win_title) {
    Window::SetMode(w, h, full_screen, win_title);
}

void Engine::Start() {
    if (!Core_Init()) {
        return;
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    SDL_Event* event = new SDL_Event;

    //const double MS_PER_UPDATE = double(1000) / _game_speed;
    Uint32 previous = SDL_GetTicks();
    double lag = 0.0;

    while (!quit)
    {
        Uint32 current = SDL_GetTicks();
        Uint32 elapsed = current - previous;
        previous = current;
        lag += elapsed;

        Surface::SetInterpolation(lag / _ms_per_update);

        Core_Render();
        Core_Event(event, keyboardState);


        while (lag > _ms_per_update)
        {
            Core_Update();
            DeleteObjects();
            lag -= _ms_per_update;

            if(lag>=1000)//debugging time skip
            {
                lag = 0;
            }

        }
    }

    delete (event);
    Core_CleanUp();
}

void Engine::Stop() {
    quit = true;
}

Object* Engine::GetRootAtLayer(unsigned int layer) {
    if(layer < _Layers.size()){
        return _Layers[layer];
    }
    return nullptr;
}

void Engine::AddLayer() {
    _Layers.resize(_Layers.size() + 1, new Object());//increase vector by 1
}



void Engine::SetGameSpeed(int ms){
    if(ms > 0){
        _ms_per_update = double(1000)/ms;
    }
}

int Engine::GetGameSpeed(){
    return 1000/_ms_per_update ;
}

void Engine::DeleteObjects(){
    if(!Object::DeleteCandidates.empty()){
        for(size_t i = 0; i < Object::DeleteCandidates.size(); ++i){
            delete(Object::DeleteCandidates[i]);
        }
        Object::DeleteCandidates.clear();
    }
}

bool Engine::Core_Init() {

	

    quit = false;

    SetGameSpeed(60);

    #ifdef USE_PHYSFS
    PhysFS::init(nullptr);

    PhysFS::mount("../Data.zip", "Data", false);
    PhysFS::mount("../Data", "Data", false);
    PhysFS::mount("../tmp", "tmp", false);

    PhysFS::setWriteDir("../tmp");
    #endif

    Resources::SetDefaultFont("BespokeSerif.ttf");
    Resources::SetDefaultFontPtsize(9);
    //Resources::SetDefaultFontStyle("pgui.png");
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
	SDL_RenderSetLogicalSize(Window::GetRenderer(), 1280, 1024); 
   

    SDL_SetRenderDrawBlendMode(Window::GetRenderer(), SDL_BLENDMODE_ADD); // https://wiki.libsdl.org/SDL_SetRenderDrawBlendMode

    Audio::Init(8);
    GUI::OnInit();
    
    if (!Window::IsInitialised()) {
		Window::SetMode(1280, 1024, false, "Lion's Den");
    }
	Background.SetTexture("Background.png");


    ImGui::CreateContext();
	    
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 16);
    //io.Fonts->TexUvScale = ImVec2(5.5f, 5.5f);
	//io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 16);
	//io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 20);
    //io.Fonts->Build();
    ImGuiSDL::Initialize(Window::GetRenderer(), Window::GetWidth(), Window::GetHeight());
    //ImGuiSDL::Deinitialize();
    
    

    Surface::BeginViewport(Vec2::ZERO, Window::GetSize());

    //Cursor::Init(Resources::GetTexture("cursor.png"), 16, 16);
    OnInit(); //CALL user function OnInit

	
    



    std::cout << "Successfully initialized!" << std::endl;
    return true; //success
}

void Engine::Core_Event(SDL_Event* event, const Uint8* keyboardState) {
    Mouse::ResetWheel();
    
    while (SDL_PollEvent(event)) {
		ImGui_ImplSDL2_ProcessEvent(event);
        bool ALT_F4 = keyboardState[SDL_SCANCODE_LALT]
                && keyboardState[SDL_SCANCODE_F4];

        Window::OnEvent(event);



        if (ALT_F4 || (event->type == SDL_QUIT)) {
            Stop();
            return;
        }

        if (event->type == SDL_MOUSEWHEEL) {
            Mouse::SetWheel(event->wheel.x, event->wheel.y);
        }
       
        

        //User OnEvent
        //if (!ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse)
        {
			OnEvent(event, keyboardState);
        }
        
    }

}

void Engine::Core_Update() {
    if (reschange == true)
    {
		RepositionGuiAtResChange();
        reschange = false;
    }
    

    GUI::SetTopObject(nullptr);
    GUI::SetTopWheeled(nullptr);


    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
    
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    //io.MouseWheel = static_cast<float>(wheel);




    if (Engine::Pause == false)
    {
		OnUpdate(); //User OnUpdate

        int length = _Layers.size();
        for(int i = 0; i < length; ++i){
            _Layers[i]->UpdateChildren();
        }

        Collider::ProcessMovementCollisions();
	    Collider::ProcessAttackCollisions();
        Audio::CalcListenersPanning();
    }
	
}

void Engine::Core_Render() {
    ImGui::NewFrame();
    
    auto bk = Window::GetBackgroundColor();
    SDL_SetRenderDrawColor(Window::GetRenderer(), bk.r, bk.b, bk.g, bk.a);
    
    SDL_RenderClear(Window::GetRenderer());

    
	Background.Draw(Vec2::ZERO+Window::GetCamera()->GetPos(),Window::GetCamera()->GetViewport(), Window::GetCamera());


    OnRender();

    


    GUI::OnUpdate();
    GUI::OnRender();

    
    OnRenderDebugGui();//render imgui debug gui




    DrawOptionsMenu();
    DrawLoadMenu();
    DrawHelpMenu();

    ImGui::EndFrame();
    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());
    //Cursor::Draw();
    SDL_RenderPresent(Window::GetRenderer());
}

void Engine::Core_CleanUp() {
    OnCleanUp(); //User CleanUp

    std::cout << "Unloading resources..." << std::endl;
    Resources::UnloadAll();

    std::cout << "Destroy objects..." << std::endl;


    std::cout << "Closing window..." << std::endl;
    Window::OnCleanUp();

    TTF_Quit();
    SDL_Quit();

     std::cout << "Quitting..." << std::endl;
}
