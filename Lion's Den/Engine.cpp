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
//Sets Game Window size
//w-width of screen 
//h-height of screen 
//full_screen- 1= enable fullscreen 0= disable fullscreen
//win_title -set title of window
//Sets Game Window size
//w-width of screen 
//h-height of screen 
//full_screen- 1= enable fullscreen 0= disable fullscreen
//win_title -set title of window
void SetVideo(int w, int h, bool full_screen, std::string win_title) {
    Window::SetMode(w, h, full_screen, win_title);
}
// Starts Engine and main loop
// Starts Engine and main loop
void Engine::Start() {
    //Initialise Core Elements
    //Initialise Core Elements
    if (!Core_Init()) {
        return;
    }

    //Initialise keyboard state variable
    //Initialise keyboard state variable
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
	//Initialise event variable
	//Initialise event variable
    SDL_Event* event = new SDL_Event;
    //Initialize variable for calculating elapsed time
    //Initialize variable for calculating elapsed time
    Uint32 previous = SDL_GetTicks();
    //Initialize lag variable;
    //Initialize lag variable;
    double lag = 0.0;
    //Main Loop of Game
    //Main Loop of Game

    


    while (!quit)
    {
        //Update current time
        //Update current time
        Uint32 current = SDL_GetTicks();
        
        //Calculate elapsed time
        
        //Calculate elapsed time
        Uint32 elapsed = current - previous;
        
        //Save timestamp for next iteration

        
        std::vector<float>& RenderFrameTimes =DebugUI::GetInstance("1")->getRenderFrameTimes();
		std::vector<float>& LogicFrameTimes =DebugUI::GetInstance("1")->getLogicFrameTimes();



		



        //Save timestamp for next iteration
        previous = current;
        
        //Calculate lag of logic update
        
        //Calculate lag of logic update
        lag += elapsed;
        
        //Update Screen function
        
        //Update Screen function






		Uint32 RenderTicksPrevious = SDL_GetTicks();
        Core_Render();
        
        //Update Event function


		Uint32 RenderTicksCurrent = SDL_GetTicks();
		Uint32 RenderTicksElapsed = RenderTicksCurrent - RenderTicksPrevious;
		

		RenderFrameTimes.push_back(RenderTicksElapsed);
        if (RenderFrameTimes.size() > 100) {
            RenderFrameTimes.erase(RenderFrameTimes.begin());
        }

        
        //Update Event function
        Core_Event(event, keyboardState);

        //Core Update Loop (possible multiple updates per frame render)
        //Core Update Loop (possible multiple updates per frame render)
        while (lag > _ms_per_update)
        {
            //Update Core calculations

            //logic




			Uint32 LogicTicksPrevious = SDL_GetTicks();
          
            Core_Update();
            
            //Delete Objects assigned for deletion

            Uint32 LogicTicksCurrent = SDL_GetTicks();
			Uint32 LogicTicksElapsed = LogicTicksCurrent - LogicTicksPrevious;
			LogicFrameTimes.push_back(LogicTicksElapsed);

            if (LogicFrameTimes.size() > 100) {
                LogicFrameTimes.erase(LogicFrameTimes.begin());
            }


            //Delete Objects assigned for deletion
            DeleteObjects();

            //calculate time 

            //calculate time 
            lag -= _ms_per_update;

            //When debugging game code, cuts down lag value to 0 if debugging induced lag is too big
            //When debugging game code, cuts down lag value to 0 if debugging induced lag is too big
            if(lag>=1000)//debugging time skip
            {
                lag = 0;
            }

        }
    }
    //Game is closing time to free used memory by program
    //Game is closing time to free used memory by program
    delete (event);
    Core_CleanUp();
}
//Flag engine to safely exit program. If engine doesn't exit by itself, it could induce a memory leaks
//Flag engine to safely exit program. If engine doesn't exit by itself, it could induce a memory leaks
void Engine::Stop() {
    quit = true;
}
//Return pointer of chosen layer nr
//Return pointer of chosen layer nr
Object* Engine::GetRootAtLayer(unsigned int layer) {
    if(layer < _Layers.size()){
        return _Layers[layer];
    }
    return nullptr;
}
//Create a new Layer and add it to layer list
//Create a new Layer and add it to layer list
void Engine::AddLayer() {
    _Layers.resize(_Layers.size() + 1, new Object());//increase vector by 1
}


//Set Game update rate in ms (desired update rate is 60ms)
//Set Game update rate in ms (desired update rate is 60ms)
void Engine::SetGameSpeed(int ms){
    if(ms > 0){
        _ms_per_update = double(1000)/ms;
    }
}
//Return current Game update rate
//Return current Game update rate
int Engine::GetGameSpeed(){
    return static_cast<int>(std::round(1000.0 / _ms_per_update));
}
//Delete objects which are assigned to be deleted
//Delete objects which are assigned to be deleted
void Engine::DeleteObjects(){
    if(!Object::DeleteCandidates.empty()){
        for(size_t i = 0; i < Object::DeleteCandidates.size(); ++i){
            delete(Object::DeleteCandidates[i]);
        }
        Object::DeleteCandidates.clear();
    }
}
//Initialize Engine Core elements
//Sets Game Speed, default font, inits SDL library, Create engine window, 
//Init Engine Audio interface, Init Engine Gui Interface,
//Init ImGui library
//Call User OnInit Function
//Initialize Engine Core elements
//Sets Game Speed, default font, inits SDL library, Create engine window, 
//Init Engine Audio interface, Init Engine Gui Interface,
//Init ImGui library
//Call User OnInit Function
bool Engine::Core_Init() {
    //Initialize quit flag
    //Initialize quit flag
    quit = false;
    
    //Set Game update rate in ms
    
    //Set Game update rate in ms
    SetGameSpeed(60);
    
    //Sets the name of font which will be used to render in game text
    //When any text is initialised Resources class will load font from Data/Fonts directory
    
    //Sets the name of font which will be used to render in game text
    //When any text is initialised Resources class will load font from Data/Fonts directory
    Resources::SetDefaultFont("BespokeSerif.ttf");
    
    //Set default font size in pixels
    
    //Set default font size in pixels
    Resources::SetDefaultFontPtsize(9);
    
    //Initialize SDL Library, if there is problem print what is the cause of it.
    //Initialize SDL Library, if there is problem print what is the cause of it.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    //Set SDL_Render logical size to which on variable resolutions SDL library will scale input
    
    //Set SDL_Render logical size to which on variable resolutions SDL library will scale input
	SDL_RenderSetLogicalSize(Window::GetRenderer(), 1280, 1024); 
    
    //Set Blend Mode used for drawing operations
    //SDL_BLENDMODE_NONE - no blending
	//SDL_BLENDMODE_BLEND - alpha blending
	//SDL_BLENDMODE_ADD - additive blending
	//SDL_BLENDMODE_MOD - color modulate
    
    //Set Blend Mode used for drawing operations
    //SDL_BLENDMODE_NONE - no blending
	//SDL_BLENDMODE_BLEND - alpha blending
	//SDL_BLENDMODE_ADD - additive blending
	//SDL_BLENDMODE_MOD - color modulate
    SDL_SetRenderDrawBlendMode(Window::GetRenderer(), SDL_BLENDMODE_ADD); // https://wiki.libsdl.org/SDL_SetRenderDrawBlendMode

    //Init Audio class used for playing sounds (value used is number of channels)
    //Overview
    //A channel is a representation of sound coming from or going to a single point. A single microphone can produce one channel of audio, and a single speaker can accept one channel of audio, for example.
    //A digital audio file can contain multiple channels of data. Music that is mixed for headphone listening is saved as a file with two channels - one sent to the left ear, one sent to the right, while surround-sound movie audio is often mixed for 6 channels.
    //Init Audio class used for playing sounds (value used is number of channels)
    //Overview
    //A channel is a representation of sound coming from or going to a single point. A single microphone can produce one channel of audio, and a single speaker can accept one channel of audio, for example.
    //A digital audio file can contain multiple channels of data. Music that is mixed for headphone listening is saved as a file with two channels - one sent to the left ear, one sent to the right, while surround-sound movie audio is often mixed for 6 channels.
    Audio::Init(8);
    //Init Gui class used for displaying Gui Widgets like in game windows,buttons etc.
    //Init Gui class used for displaying Gui Widgets like in game windows,buttons etc.
    GUI::OnInit();
    std::ifstream cFile ("config.txt");
    int w,h;
    if (cFile.is_open())
    {
        std::string line;
        while(getline(cFile, line))
       {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                 line.end());
            if( line.empty() || line[0] == '#' )
            {
                continue;
            }
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            if (name == "w")
            {
                w = std::stoi(value.c_str());
            }
			if (name == "h")
            {
                h = std::stoi(value.c_str());
            }

            std::cout << name << " " << value << '\n';
        }
    }
    else 
    {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    //Initialise Window Class and create a Game Main Window used for rendering
    std::ifstream cFile ("config.txt");
    int w,h;
    if (cFile.is_open())
    {
        std::string line;
        while(getline(cFile, line))
       {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                 line.end());
            if( line.empty() || line[0] == '#' )
            {
                continue;
            }
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            if (name == "w")
            {
                w = std::stoi(value.c_str());
            }
			if (name == "h")
            {
                h = std::stoi(value.c_str());
            }

            std::cout << name << " " << value << '\n';
        }
    }
    else 
    {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    //Initialise Window Class and create a Game Main Window used for rendering
    if (!Window::IsInitialised()) {
		Window::SetMode(w, h, false, "Lion's Den");
		Window::SetMode(w, h, false, "Lion's Den");
    }
    //Set Background default texture. 
    //Set Background default texture. 
	Background.SetTexture("Background.png");

    //Initialise context of ImGui Library
    //Initialise context of ImGui Library
    ImGui::CreateContext();
	    
    //Get ImGui IO Structure in case of editing some of its values
    //Get ImGui IO Structure in case of editing some of its values
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 16);
    //io.Fonts->TexUvScale = ImVec2(5.5f, 5.5f);
	//io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 16);
	//io.Fonts->AddFontFromFileTTF("Data/Fonts/BespokeSerif.ttf", 20);
    //io.Fonts->Build();
    
    //Initialise ImGui SDL Library wrapper, without it SDL cant render ImGui on screen
    
    //Initialise ImGui SDL Library wrapper, without it SDL cant render ImGui on screen
    ImGuiSDL::Initialize(Window::GetRenderer(), Window::GetWidth(), Window::GetHeight());
    //ImGuiSDL::Deinitialize();
    
    
    //Initialise Viewport of window
    //Initialise Viewport of window
    Surface::BeginViewport(Vec2::ZERO, Window::GetSize());

    //Set cursor image
    //Set cursor image
    //Cursor::Init(Resources::GetTexture("cursor.png"), 16, 16);
    
	//CALL user function OnInit
    OnInit(); 
	//CALL user function OnInit
    OnInit(); 

    //If everything is initialised print message
    //If everything is initialised print message
    std::cout << "Successfully initialized!" << std::endl;
    return true; //success
}
//Process Core Events
//Core events are: Keyboard and Mouse buttons press.
//Call User OnEvent Function
//Process Core Events
//Core events are: Keyboard and Mouse buttons press.
//Call User OnEvent Function
void Engine::Core_Event(SDL_Event* event, const Uint8* keyboardState) {
    //Reset Mouse Wheel position used for scrolling widgets
    //Reset Mouse Wheel position used for scrolling widgets
    Mouse::ResetWheel();
    
    //Event processing Loop
    //Event processing Loop
    while (SDL_PollEvent(event)) {
        //First Pass any event to ImGui Library
        //First Pass any event to ImGui Library
		ImGui_ImplSDL2_ProcessEvent(event);
        
        //Get ALT+F4 key combination in case of exiting game in this way
        
        //Get ALT+F4 key combination in case of exiting game in this way
        bool ALT_F4 = keyboardState[SDL_SCANCODE_LALT]
                && keyboardState[SDL_SCANCODE_F4];

        //Pass any event to Window class (used ie: pressing engine buttons)
        //Pass any event to Window class (used ie: pressing engine buttons)
        Window::OnEvent(event);

        //if ALT+F4 combination or window X button is pressed, redirect program to safely terminate
        //if ALT+F4 combination or window X button is pressed, redirect program to safely terminate
        if (ALT_F4 || (event->type == SDL_QUIT)) {
            Stop();
            return;
        }

        //Update Mouse class on SDL_MOUSEWHEEL event
        //Update Mouse class on SDL_MOUSEWHEEL event
        if (event->type == SDL_MOUSEWHEEL) {
            Mouse::SetWheel(event->wheel.x, event->wheel.y);
        }

        //Pass other event to User OnEvent function
		OnEvent(event, keyboardState);//CALL user function OnEvent

        //Pass other event to User OnEvent function
		OnEvent(event, keyboardState);//CALL user function OnEvent
    }

}
//Update Engine in game core loop
//Call User OnUpdate Function
//Update Engine in game core loop
//Call User OnUpdate Function
void Engine::Core_Update() {
    //On core update detect if program needs to change resulution
    //On core update detect if program needs to change resulution
    if (reschange == true)
    {
        //Reposition Gui Elements and calculate its size according to screen resolution
        //Reposition Gui Elements and calculate its size according to screen resolution
		RepositionGuiAtResChange();

        //Gui Elements updated reset reschange flag to false

        //Gui Elements updated reset reschange flag to false
        reschange = false;
    }
    
    //Reset Pointer to last hovered widget
    //Reset Pointer to last hovered widget
    GUI::SetTopObject(nullptr);
    //Reset Pointer to last wheeled widget
    //Reset Pointer to last wheeled widget
    GUI::SetTopWheeled(nullptr);

    //Pass Mouse data to ImGui
        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
    //Pass Mouse data to ImGui
        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

        // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
        // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
    
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f / 60.0f;
        io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        //io.MouseWheel = static_cast<float>(wheel);
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = 1.0f / 60.0f;
        io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        //io.MouseWheel = static_cast<float>(wheel);

    //if game is not paused update game data
    //if game is not paused update game data
    if (Engine::Pause == false)
    {
        //Update game data

		std::vector<float>& FrameTimes =DebugUI::GetInstance("1")->getFrameTimes();
		
		
        //test here


	
	
        //Update game data
		OnUpdate(); //User OnUpdate

        //Update Layers an their connected childrens
        int length = _Layers.size();
        for(int i = 0; i < length; ++i){
		

        for(int i = 0; i < _Layers.size(); ++i){
            _Layers[i]->UpdateChildren();
        }
        //Process game movement collisions
		



			Uint32 TicksPrevious = SDL_GetTicks();
        Collider::ProcessMovementCollisions();
	    
		//Process game attack collisions
        Collider::ProcessAttackCollisions();

        //Calculate Audio Panning
			Uint32 TicksCurrent = SDL_GetTicks();
		Uint32 TicksElapsed = TicksCurrent - TicksPrevious;
		FrameTimes.push_back(TicksElapsed);
        if(FrameTimes.size() > 100) {FrameTimes.erase(FrameTimes.begin());}
        Collider::ProcessAttackCollisions();

        Audio::CalcListenersPanning();
    }
	
}
//Update Engine in game render loop
//
//Call User OnRender Function
void Engine::Core_Render() {
    //ImGui needs a new frame to render in each iteration
    ImGui::NewFrame();
    
    //Set default color of SDL drawing operations

    auto bk = Window::GetBackgroundColor();
    SDL_SetRenderDrawColor(Window::GetRenderer(), bk.r, bk.b, bk.g, bk.a);
    
    //Clear window renderer with default drawing color
    SDL_RenderClear(Window::GetRenderer());

    //Render Background texture on window

	Background.Draw(Vec2::ZERO+Window::GetCamera()->GetPos(),Window::GetCamera()->GetViewport(), Window::GetCamera());

    //Call User OnRender function (render game data)

    OnRender();
    
    //Call GUI class onRender function
    GUI::OnUpdate();

    //Call GUI class onRender function
    GUI::OnRender();

    //Render ImGui user defined window
    OnRenderDebugGui();//render imgui debug gui

    //Draw Options Menu
    OnRenderDebugGui();
    DrawOptionsMenu();

    //Draw Load Game Menu
    DrawLoadMenu();

    //Draw Help Menu
    DrawHelpMenu();

    //Tell ImGui its end of ImGui rendering calls
    ImGui::EndFrame();
    //Compile all ImGui calls and render it
    ImGui::Render();
    //Translate ImGui rendered data to SDL Library
    ImGuiSDL::Render(ImGui::GetDrawData());

    //Draw Cursor image on top of everything
    //Cursor::Draw();
    
    //Pass on Screen everything that was rendered until now
    SDL_RenderPresent(Window::GetRenderer());
}

//Unload and delete everything that engine used
//Unload and delete everything that engine used
void Engine::Core_CleanUp() {
    OnCleanUp(); //User CleanUp

    //Unload All Resources
    std::cout << "Unloading resources..." << std::endl;
    Resources::UnloadAll();

    //Destroy All Objects
    std::cout << "Destroy objects..." << std::endl;

    //Close Window and delete data
    std::cout << "Closing window..." << std::endl;
    Window::OnCleanUp();

    //Deinitialise font engine
    TTF_Quit();
    //Deinitialise SDL Library
    SDL_Quit();

     std::cout << "Quitting..." << std::endl;
}
