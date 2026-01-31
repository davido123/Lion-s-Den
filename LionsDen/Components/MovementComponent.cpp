#include "MovementComponent.h"

MovementComponent::MovementComponent(Object* owner, float maxSpeed)
    : Component(owner)
    , _maxSpeed(maxSpeed)
{
}

void MovementComponent::OnUpdate() {
    // Movement logic can be moved here from Player later
    // (e.g. apply velocity to owner position, pursue target)
}
