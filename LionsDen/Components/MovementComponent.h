#pragma once

#include "Component.h"
#include "../Core/Vec2.h"

/**
 * MovementComponent - Movement state and behavior (velocity, target)
 * Stub for future extraction of movement logic from Player
 */
class MovementComponent : public Component {
public:
    MovementComponent(Object* owner, float maxSpeed = 2.0f);

    float GetMaxSpeed() const { return _maxSpeed; }
    void SetMaxSpeed(float s) { _maxSpeed = s; }
    Vec2 GetMoveTarget() const { return _moveTarget; }
    void SetMoveTarget(const Vec2& t) { _moveTarget = t; }

    void OnUpdate() override;

private:
    float _maxSpeed;
    Vec2 _moveTarget;
};
