#include "HealthComponent.h"
#include "../Systems/EventSystem.h"
#include "../Player.h"
#include <algorithm>

HealthComponent::HealthComponent(Object* owner, int maxHealth)
    : Component(owner)
    , _health(maxHealth)
    , _maxHealth(maxHealth)
{
}

void HealthComponent::SetHealth(int value) {
    _health = std::clamp(value, 0, _maxHealth);
    Player* player = dynamic_cast<Player*>(_owner);
    if (player) {
        HealthChangedEvent evt;
        evt.player = player;
        EventSystem::GetInstance().Emit("HealthChanged", &evt);
    }
}

void HealthComponent::SetMaxHealth(int value) {
    _maxHealth = value > 0 ? value : 1;
    _health = std::min(_health, _maxHealth);
}

void HealthComponent::TakeDamage(int damage) {
    _health = std::max(0, _health - damage);
    Player* player = dynamic_cast<Player*>(_owner);
    if (player) {
        HealthChangedEvent evt;
        evt.player = player;
        EventSystem::GetInstance().Emit("HealthChanged", &evt);
    }
}

void HealthComponent::Heal(int amount) {
    _health = std::min(_maxHealth, _health + amount);
    Player* player = dynamic_cast<Player*>(_owner);
    if (player) {
        HealthChangedEvent evt;
        evt.player = player;
        EventSystem::GetInstance().Emit("HealthChanged", &evt);
    }
}

void HealthComponent::OnUpdate() {
    // Placeholder for regen, etc.
}
