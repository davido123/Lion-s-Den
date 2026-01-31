#pragma once

#include "Scene.h"
#include "../Player.h"
#include "../Map.h"
#include "../Monster.h"
#include "../Core/Object.h"
#include "../GUI/Widgets/StatsBar.h"
#include "../GUI/Widgets/Inventory.h"
#include "../Systems/EventSystem.h"

/**
 * GameScene - Handles the main gameplay state
 * Manages player, monsters, map, and game logic
 */
class GameScene : public Scene {
public:
    GameScene();
    ~GameScene() override = default;

    void Enter() override;
    void Exit() override;
    void HandleEvent(SDL_Event* event, const Uint8* keyboardState) override;
    void Update() override;
    void Render() override;
    void RenderImGui() override;
    const std::string& GetName() const override { return _name; }

    // Access to game objects (for Game class compatibility during migration)
    Player* GetPlayer() const { return _player; }
    Map* GetMap() const { return _map; }
    StatsBar* GetStatsBar() const { return _statsBar; }
    Inventory* GetInventory() const { return _inventory; }
    bool IsDrawCollisions() const { return _drawCollisions; }
    void SetDrawCollisions(bool draw) { _drawCollisions = draw; }

private:
    static const std::string _name;
    
    // Game objects
    Player* _player;
    Map* _map;
    
    // GUI elements
    StatsBar* _statsBar;
    Inventory* _inventory;
    
    // Layers
    Object* _layerGui;
    Object* _layerPlayer;
    
    // Game state
    bool _initialized;
    bool _drawCollisions;

    // Event subscriptions (unsubscribe in Exit)
    EventSystem::EventID _playerDeathSubscription = 0;
    EventSystem::EventID _monsterKilledSubscription = 0;
    EventSystem::EventID _healthChangedSubscription = 0;
    EventSystem::EventID _manaChangedSubscription = 0;
    EventSystem::EventID _experienceChangedSubscription = 0;

    void SetupGame();
    void CleanupGame();
    void HandlePlayStateEvents(SDL_Event* event);
    void HandlePlayerDeath(Player* player);
    void HandleMonsterKilled(Monster* monster, Player* killer);
};
