#pragma once

#include "Component.h"

/**
 * HealthComponent - Health/max health, damage, heal, death
 * Emits HealthChangedEvent on change when owner is Player
 */
class HealthComponent : public Component {
public:
    HealthComponent(Object* owner, int maxHealth);

    int GetHealth() const { return _health; }
    int GetMaxHealth() const { return _maxHealth; }
    void SetHealth(int value);
    void SetMaxHealth(int value);
    void TakeDamage(int damage);
    void Heal(int amount);
    bool IsDead() const { return _health <= 0; }

    void OnUpdate() override;

private:
    int _health;
    int _maxHealth;
};
