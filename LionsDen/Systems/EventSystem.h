#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

// Forward declarations for event payloads
class Player;
class Monster;
class Item;

/**
 * EventSystem - Decouples game objects via publish/subscribe events
 * Singleton; subscribe with callback, emit by name, unsubscribe by ID
 */
class EventSystem {
public:
    using EventCallback = std::function<void(void*)>;
    using EventID = size_t;

    static EventSystem& GetInstance();

    EventID Subscribe(const std::string& eventName, EventCallback callback);
    void Unsubscribe(const std::string& eventName, EventID id);
    void Emit(const std::string& eventName, void* data = nullptr);
    void Clear();

private:
    EventSystem() = default;
    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;

    struct Subscription {
        EventID id;
        EventCallback callback;
    };

    std::unordered_map<std::string, std::vector<Subscription>> _listeners;
    EventID _nextID = 1;
};

// Event payload structs (pointers only; include full types at use site)
struct PlayerDeathEvent {
    Player* player;
};

struct MonsterKilledEvent {
    Monster* monster;
    Player* killer;
};

struct ItemPickedUpEvent {
    Item* item;
    Player* player;
};

struct SceneChangeEvent {
    std::string fromScene;
    std::string toScene;
};

struct HealthChangedEvent {
    Player* player;
};

struct ManaChangedEvent {
    Player* player;
};

struct ExperienceChangedEvent {
    Player* player;
};

struct InventoryChangedEvent {
    Player* player;
};

struct StatsChangedEvent {
    Player* player;
};
