#include "GameScene.h"
#include "../Engine.h"
#include "../Core/Collider.h"
#include "../Core/Object.h"
#include "../Core/Resources.h"
#include "../Core/Vec2.h"
#include "../GUI/Widgets/StatsBar.h"
#include "../GUI/Widgets/Inventory.h"
#include "../IO/Keyboard.h"
#include "../IO/Mouse.h"
#include "../ImGui/imgui.h"
#include "../Render/Drawer.h"
#include "../Render/Camera.h"
#include "../GUI/Window.h"
#include "../Monster.h"
#include "../Item.h"
#include "../Player.h"
#include "../Map.h"
#include "../Systems/SceneManager.h"
#include "../Systems/EventSystem.h"
#include "../Game/Game.h"
#include <iostream>

const std::string GameScene::_name = "Game";

GameScene::GameScene()
    : _player(nullptr)
    , _map(nullptr)
    , _statsBar(nullptr)
    , _inventory(nullptr)
    , _layerGui(nullptr)
    , _layerPlayer(nullptr)
    , _initialized(false)
    , _drawCollisions(false)
{
}

void GameScene::Enter() {
    std::cout << "GameScene: Entering game scene" << std::endl;
    if (!_initialized) {
        SetupGame();
        _initialized = true;
    }

    // Subscribe to player death (decoupled via EventSystem)
    _playerDeathSubscription = EventSystem::GetInstance().Subscribe("PlayerDeath",
        [this](void* data) {
            PlayerDeathEvent* evt = static_cast<PlayerDeathEvent*>(data);
            if (evt && evt->player) {
                HandlePlayerDeath(evt->player);
            }
        });

    // Subscribe to monster killed (e.g. XP, loot)
    _monsterKilledSubscription = EventSystem::GetInstance().Subscribe("MonsterKilled",
        [this](void* data) {
            MonsterKilledEvent* evt = static_cast<MonsterKilledEvent*>(data);
            if (evt && evt->monster) {
                HandleMonsterKilled(evt->monster, evt->killer);
            }
        });

    // Subscribe to health/stats changes so UI updates via events (no direct Player->StatsBar)
    _healthChangedSubscription = EventSystem::GetInstance().Subscribe("HealthChanged",
        [this](void* data) {
            HealthChangedEvent* evt = static_cast<HealthChangedEvent*>(data);
            if (evt && evt->player && _statsBar && evt->player == _player) {
                _statsBar->UpdateValues(evt->player);
            }
        });
    _manaChangedSubscription = EventSystem::GetInstance().Subscribe("ManaChanged",
        [this](void* data) {
            ManaChangedEvent* evt = static_cast<ManaChangedEvent*>(data);
            if (evt && evt->player && _statsBar && evt->player == _player) {
                _statsBar->UpdateValues(evt->player);
            }
        });
    _experienceChangedSubscription = EventSystem::GetInstance().Subscribe("ExperienceChanged",
        [this](void* data) {
            ExperienceChangedEvent* evt = static_cast<ExperienceChangedEvent*>(data);
            if (evt && evt->player && _statsBar && evt->player == _player) {
                _statsBar->UpdateValues(evt->player);
            }
        });

    // Show gameplay UI
    if (_statsBar) {
        _statsBar->Show(true);
    }
    if (_inventory) {
        _inventory->Show(false); // Hidden by default, toggle with 'I'
    }
}

void GameScene::Exit() {
    std::cout << "GameScene: Exiting game scene" << std::endl;
    // Unsubscribe from events
    if (_playerDeathSubscription != 0) {
        EventSystem::GetInstance().Unsubscribe("PlayerDeath", _playerDeathSubscription);
        _playerDeathSubscription = 0;
    }
    if (_monsterKilledSubscription != 0) {
        EventSystem::GetInstance().Unsubscribe("MonsterKilled", _monsterKilledSubscription);
        _monsterKilledSubscription = 0;
    }
    if (_healthChangedSubscription != 0) {
        EventSystem::GetInstance().Unsubscribe("HealthChanged", _healthChangedSubscription);
        _healthChangedSubscription = 0;
    }
    if (_manaChangedSubscription != 0) {
        EventSystem::GetInstance().Unsubscribe("ManaChanged", _manaChangedSubscription);
        _manaChangedSubscription = 0;
    }
    if (_experienceChangedSubscription != 0) {
        EventSystem::GetInstance().Unsubscribe("ExperienceChanged", _experienceChangedSubscription);
        _experienceChangedSubscription = 0;
    }
    // Hide gameplay UI
    if (_statsBar) {
        _statsBar->Show(false);
    }
    if (_inventory) {
        _inventory->Show(false);
    }
    // Note: Game objects are cleaned up in CleanupGame() when scene is destroyed
}

void GameScene::HandlePlayerDeath(Player* player) {
    (void)player;
    // Death is handled by RenderImGui() (death dialog when _player->isDead).
    // Use this for future logic: scene switch, stats, save, etc.
}

void GameScene::HandleMonsterKilled(Monster* monster, Player* killer) {
    (void)monster;
    (void)killer;
    // Placeholder for XP, loot UI, achievements, etc. killer may be nullptr until last-attacker is tracked.
}

void GameScene::CleanupGame() {
    // Cleanup game objects if they were created by this scene
    // Note: During migration, objects might be shared with Game class
    // So we only cleanup if we're sure they're ours
    if (_initialized) {
        // Objects will be cleaned up by their destructors when scene is destroyed
        // or by Game class if they're shared
        _player = nullptr;
        _map = nullptr;
        _statsBar = nullptr;
        _inventory = nullptr;
        _initialized = false;
    }
}

void GameScene::SetupGame() {
    if (!_engine) {
        std::cerr << "GameScene: Engine not available" << std::endl;
        return;
    }
    
    // Ensure layers exist (create if needed)
    if (_engine->GetRootAtLayer(0) == nullptr) {
        _engine->AddLayer();
        _engine->AddLayer();
        _engine->AddLayer();
    }
    
    // Get layers from engine
    _layerPlayer = _engine->GetRootAtLayer(0);
    _layerGui = _engine->GetRootAtLayer(1);
    
    if (!_layerPlayer || !_layerGui) {
        std::cerr << "GameScene: Failed to get layers" << std::endl;
        return;
    }
    
    _layerPlayer->_custom_render = true;
    
    // Create map
    if (!_map) {
        _map = new Map();
        _map->LoadJson("Data/Maps/Map.json");
        _map->GetCollisionBoxes();
        _map->RegisterCollisionBoxes();
    }
    
    // Use Game's player if it already exists (from OnGameInit); avoid creating a second player
    // which would double-register with Collider and create phantom/collision conflicts
    if (!_player) {
        Player* gamePlayer = Game::GetInstance().GetPlayer();
        if (gamePlayer) {
            _player = gamePlayer;
            // Already registered with Collider and connected to layer by Game::OnGameInit
        } else {
            _player = new Player();
            Collider::RegisterObject(_player, _player->GetColliderPos(), _player->GetColliderBox(), false);
            Collider::RegisterTarget(_player, _player->GetColliderPos(), _player->GetColliderBox());
            _layerPlayer->Connect(_player);
        }
    }
    
    // Create StatsBar
    if (!_statsBar) {
        _statsBar = new StatsBar(Vec2(0, 0), Vec2(300, 64));
        _statsBar->SetStyle("GUI/hud.png");
        _statsBar->ConnectPlayer(_player);
        _layerGui->Connect(_statsBar);
        _statsBar->Show(false);
    }
    
    // Create Inventory
    if (!_inventory) {
        _inventory = new Inventory(Vec2(80, 40), Vec2(35, 60), Resources::GetDefaultFont(), 16);
        _inventory->ConnectPlayer(_player);
        _layerGui->Connect(_inventory);
        _inventory->Show(false);
    }
    
    std::cout << "GameScene: Game setup complete (objects created)" << std::endl;
}

void GameScene::HandleEvent(SDL_Event* event, const Uint8* keyboardState) {
    if (!event) return;
    
    HandlePlayStateEvents(event);
}

void GameScene::HandlePlayStateEvents(SDL_Event* event) {
    if (!_player || !_inventory) return;
    
    if (event->type == SDL_KEYDOWN) {
        if (Keyboard::isKeyDown(KEY_I)) {
            if (_inventory) {
                _inventory->Show(!_inventory->IsVisible());
                _inventory->SetRenderItems(_inventory->IsVisible());
            }
        }
        if (Keyboard::isKeyDown(KEY_C)) {
            if (_player) {
                _player->drawStatBox = !_player->drawStatBox;
            }
        }
    }
    
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (_inventory) {
            _inventory->_mbtpressed = true;
            _inventory->_mbtreleased = false;
        }
    }
    
    if (event->type == SDL_MOUSEBUTTONUP) {
        if (_inventory) {
            _inventory->_mbtpressed = false;
            _inventory->_mbtreleased = true;
        }
    }
}

void GameScene::Update() {
    if (!_player || !_map) return;
    
    // Update ImGui key mappings
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    
    // Update monsters to target player
    for (auto monster : Monster::MonsterList) {
        if (monster && _player) {
            monster->SetMoveTarget(_player->GetPos());
        }
    }
    
    // Update camera to follow player
    if (_player && _map && _engine) {
        Window::GetCamera()->CenterOnPlayer(_player->GetPos(), _player->GetSize(), _map->GetMapSizePixels());
    }
    
    // Pause and death handling moved to RenderImGui()
}

void GameScene::Render() {
    if (!_map) return;
    
    // Render map layers
    _map->DrawLayer("Floor");
    _map->DrawLayer("Floor2");
    _map->DrawLayerOrderNR("Walls", 0);
    
    // Render player and monsters (layer 0)
    if (_engine && _layerPlayer) {
        _layerPlayer->RenderChildren();
    }
    
    _map->DrawLayerOrderNR("Walls", 1);
    
    // Render collisions if debug mode
    if (_drawCollisions) {
        Collider::DrawCollisions();
    }
    
    // Render GUI layer (layer 1)
    if (_engine && _layerGui) {
        _layerGui->RenderChildren();
    }
    
    // Finalize rendering
    Drawer::RenderAll(Window::GetRenderer(), Window::GetCamera());
}

void GameScene::RenderImGui() {
    // Render pause menu
    if (Engine::Pause) {
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        
        ImGui::OpenPopup("Pause");
        if (ImGui::BeginPopupModal("Pause", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Do you want to exit and save game?");
            ImGui::Separator();
            
            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                // Save game and return to main menu
                Game::GetInstance().SaveGame();
                Engine::Pause = false;
                // Switch to main menu scene
                SceneManager::GetInstance().SetActiveScene("MainMenu");
                ImGui::CloseCurrentPopup();
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
    
    // Render death dialog
    if (_player && _player->isDead) {
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        
        ImGui::OpenPopup("Player Death");
        if (ImGui::BeginPopupModal("Player Death", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("You have died!");
            ImGui::Separator();
            ImGui::Text("Return to main menu?");
            ImGui::Separator();
            
            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                // Return to main menu
                SceneManager::GetInstance().SetActiveScene("MainMenu");
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            
            if (ImGui::Button("No", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
}
