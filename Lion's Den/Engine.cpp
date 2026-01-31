/*!
 boldtemple Gaming ©, http://gaming.boldtemple.net
 OpenSource Project - Check Development, License & Readme Sections.

 BGE - boldtemple Cross Platform Game Engine
 /Engine.cpp
 !*/
#include "Engine.h"
#include "Systems/SceneManager.h"
#include "Systems/ImGuiManager.h"

Engine::Engine() {
}

Engine::~Engine() {
}

void Engine::SetVideo(int w, int h, bool full_screen, std::string win_title) {
    Window::SetMode(w, h, full_screen, win_title);
}
// Starts Engine and main loop
void Engine::Start() {
    // Initialise Core Elements
    if (!Core_Init()) {
        return;
    }
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
    SDL_Event event;
    Uint32 previous = SDL_GetTicks();
    double lag = 0.0;
    while (!quit)
    {

        Uint32 current = SDL_GetTicks();
        Uint32 elapsed = current - previous;
        std::vector<float>& RenderFrameTimes =DebugUI::GetInstance("1")->getRenderFrameTimes();
		std::vector<float>& LogicFrameTimes =DebugUI::GetInstance("1")->getLogicFrameTimes();

        previous = current;
        lag += elapsed;

		Uint32 RenderTicksPrevious = SDL_GetTicks();
        Core_Render();
		Uint32 RenderTicksCurrent = SDL_GetTicks();
		Uint32 RenderTicksElapsed = RenderTicksCurrent - RenderTicksPrevious;

		RenderFrameTimes.push_back(RenderTicksElapsed);
        if (RenderFrameTimes.size() > 100) {
            RenderFrameTimes.erase(RenderFrameTimes.begin());
        }

        
        // Update Event function
        Core_Event(&event, keyboardState);

        // Core Update Loop (possible multiple updates per frame render)
        while (lag >= _ms_per_update)
        {
            Uint32 LogicTicksPrevious = SDL_GetTicks();
            Core_Update();
            Uint32 LogicTicksCurrent = SDL_GetTicks();
            Uint32 LogicTicksElapsed = LogicTicksCurrent - LogicTicksPrevious;
            
            LogicFrameTimes.push_back(static_cast<float>(LogicTicksElapsed));
            if (LogicFrameTimes.size() > 100) {
                LogicFrameTimes.erase(LogicFrameTimes.begin());
            }

            DeleteObjects();
            lag -= _ms_per_update;

            // When debugging game code, cuts down lag value to 0 if debugging induced lag is too big
            if (lag >= 1000.0) { // debugging time skip
                lag = 0.0;
            }
        }
    }
    // Game is closing time to free used memory by program
    Core_CleanUp();
}

// Flag engine to safely exit program. If engine doesn't exit by itself, it could induce a memory leaks
void Engine::Stop() {
    quit = true;
}
// Return pointer of chosen layer nr
Object* Engine::GetRootAtLayer(unsigned int layer) {
    if (layer < _Layers.size()) {
        return _Layers[layer];
    }
    return nullptr;
}

// Create a new Layer and add it to layer list
void Engine::AddLayer() {
    _Layers.push_back(new Object());
}

// Set Game update rate in ms (desired update rate is 60ms)
void Engine::SetGameSpeed(int ms) {
    if (ms > 0) {
        _ms_per_update = 1000.0 / ms;
    }
}

// Return current Game update rate
int Engine::GetGameSpeed() {
    return static_cast<int>(std::round(1000.0 / _ms_per_update));
}

// Delete objects which are assigned to be deleted
void Engine::DeleteObjects() {
    if (!Object::DeleteCandidates.empty()) {
        for (size_t i = 0; i < Object::DeleteCandidates.size(); ++i) {
            delete Object::DeleteCandidates[i];
        }
        Object::DeleteCandidates.clear();
    }
}

bool Engine::Core_Init() {

    quit = false;
    SetGameSpeed(60);
    Resources::SetDefaultFont("BespokeSerif.ttf");
    Resources::SetDefaultFontPtsize(9);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_RenderSetLogicalSize(Window::GetRenderer(), 1280, 1024); 
    
    SDL_SetRenderDrawBlendMode(Window::GetRenderer(), SDL_BLENDMODE_ADD); 

    Audio::Init(8);
    GUI::OnInit();
    
    // Read config file for window dimensions
    int w = 1280, h = 1024; // Default values
    std::ifstream cFile("config.txt");
    if (cFile.is_open()) {
        std::string line;
        while (getline(cFile, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            if (line.empty() || line[0] == '#') {
                continue;
            }
            auto delimiterPos = line.find("=");
            if (delimiterPos == std::string::npos) {
                continue;
            }
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            if (name == "w") {
                w = std::stoi(value);
            } else if (name == "h") {
                h = std::stoi(value);
            }
            std::cout << name << " " << value << '\n';
        }
        cFile.close();
    } else {
        std::cerr << "Couldn't open config file for reading, using defaults.\n";
    }
    
    // Initialise Window Class and create a Game Main Window used for rendering
    if (!Window::IsInitialised()) {
        Window::SetMode(w, h, false, "Lion's Den");
    }
    // Set Background default texture
    Background.SetTexture("Background.png");
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Enable ImGui .ini file for window state persistence
    io.IniFilename = "imgui.ini";
    
    // Setup custom ImGui style
    {
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Customize colors - Dark theme with game-specific accents
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.95f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.50f, 0.70f, 1.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.70f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.80f, 1.00f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.30f, 1.00f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.30f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.55f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.60f, 1.00f);
        
        // Customize sizes and rounding
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 3.0f;
        style.ChildRounding = 3.0f;
        style.PopupRounding = 5.0f;
        
        // Spacing
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
        
        // Borders
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
    }
    
    // Initialize ImGuiManager (replaces direct ImGuiSDL initialization)
    ImGuiManager::GetInstance().Initialize(Window::GetRenderer(), Window::GetWidth(), Window::GetHeight());
    Surface::BeginViewport(Vec2::ZERO, Window::GetSize());

    OnInit();

    std::cout << "Successfully initialized!" << std::endl;
    return true; // success
}

void Engine::Core_Event(SDL_Event* event, const Uint8* keyboardState) {
    // Reset Mouse Wheel position used for scrolling widgets
    Mouse::ResetWheel();
    
    // Event processing Loop
    while (SDL_PollEvent(event)) {
        // First Pass any event to ImGui Library
        ImGui_ImplSDL2_ProcessEvent(event);
        
        // Get ALT+F4 key combination in case of exiting game in this way
        bool ALT_F4 = keyboardState[SDL_SCANCODE_LALT] && keyboardState[SDL_SCANCODE_F4];

        // Pass any event to Window class (used ie: pressing engine buttons)
        Window::OnEvent(event);

        // If ALT+F4 combination or window X button is pressed, redirect program to safely terminate
        if (ALT_F4 || (event->type == SDL_QUIT)) {
            Stop();
            return;
        }

        // Update Mouse class on SDL_MOUSEWHEEL event
        if (event->type == SDL_MOUSEWHEEL) {
            Mouse::SetWheel(event->wheel.x, event->wheel.y);
        }

        // Pass event to SceneManager (if scenes are active)
        SceneManager::GetInstance().HandleEvent(event, keyboardState);
        
        // Pass other event to User OnEvent function (for backward compatibility)
        OnEvent(event, keyboardState);
    }
}
// Update Engine in game core loop
// Call User OnUpdate Function
void Engine::Core_Update() {
    // On core update detect if program needs to change resolution
    if (reschange == true)
    {
        // Reposition Gui Elements and calculate its size according to screen resolution
        RepositionGuiAtResChange();
        // Gui Elements updated reset reschange flag to false
        reschange = false;
    }
    
    // Reset Pointer to last hovered widget
    GUI::SetTopObject(nullptr);
    // Reset Pointer to last wheeled widget
    GUI::SetTopWheeled(nullptr);
    
    // Note: WantCaptureMouse is updated in NewFrame() (render phase), so it's from the previous frame
    // We check it here to prevent GUI widgets from being set as top object when ImGui is capturing
    // This ensures GUI clicks are properly blocked when ImGui windows are open

    // Pass Mouse data to ImGui
    int mouseX, mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    // Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    // io.MouseWheel = static_cast<float>(wheel);

    //if game is not paused update game data
    //if game is not paused update game data
    if (Engine::Pause == false)
    {
        //Update game data

		std::vector<float>& FrameTimes =DebugUI::GetInstance("1")->getFrameTimes();
		
		
        //test here


	
	
        //Update game data
		OnUpdate(); //User OnUpdate

        // Update Layers and their connected children
        for (auto* layer : _Layers) {
            if (layer) {
                layer->UpdateChildren();
            }
        }
        // Process game movement collisions
        Uint32 TicksPrevious = SDL_GetTicks();
        Collider::ProcessMovementCollisions();
        
        // Process game attack collisions
        Collider::ProcessAttackCollisions();

        // Calculate Audio Panning
        Uint32 TicksCurrent = SDL_GetTicks();
        Uint32 TicksElapsed = TicksCurrent - TicksPrevious;
        FrameTimes.push_back(static_cast<float>(TicksElapsed));
        if (FrameTimes.size() > 100) {
            FrameTimes.erase(FrameTimes.begin());
        }

        Audio::CalcListenersPanning();
    }
}
//Update Engine in game render loop
//
//Call User OnRender Function
void Engine::Core_Render() {
    // Begin ImGui frame (managed by ImGuiManager)
    ImGuiManager::GetInstance().BeginFrame();
    
    //Set default color of SDL drawing operations

    auto bk = Window::GetBackgroundColor();
    SDL_SetRenderDrawColor(Window::GetRenderer(), bk.r, bk.g, bk.b, bk.a);
    
    //Clear window renderer with default drawing color
    SDL_RenderClear(Window::GetRenderer());

    //Render Background texture on window

	Background.Draw(Vec2::ZERO+Window::GetCamera()->GetPos(),Window::GetCamera()->GetViewport(), Window::GetCamera());

    // Render active scene (if scenes are being used)
    SceneManager::GetInstance().Render();
    
    // Call User OnRender function (render game data - for backward compatibility)
    OnRender();
    
    //Call GUI class onRender function
    GUI::OnUpdate();

    //Call GUI class onRender function
    GUI::OnRender();

    // Render ImGui UI for active scene (scene-based ImGui rendering)
    SceneManager::GetInstance().RenderImGui();

    // Render ImGui user defined window (global debug UI)
    OnRenderDebugGui(); // render imgui debug gui

    // End ImGui frame and render (managed by ImGuiManager)
    ImGuiManager::GetInstance().EndFrame();
    ImGuiManager::GetInstance().Render();

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
    // Shutdown ImGuiManager
    ImGuiManager::GetInstance().Shutdown();
    
    //Deinitialise SDL Library
    SDL_Quit();

     std::cout << "Quitting..." << std::endl;
}
