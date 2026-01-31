#pragma once

#include <SDL.h>
#include "../Core/Object.h"

/**
 * Component - Base class for composable behavior attached to an Object
 * Override OnUpdate, OnRender, OnEvent as needed
 */
class Component {
public:
    Component(Object* owner);
    virtual ~Component();

    virtual void OnUpdate() {}
    virtual void OnRender() {}
    virtual void OnEvent(SDL_Event* event) {}

    Object* GetOwner() const { return _owner; }
    bool IsEnabled() const { return _enabled; }
    void SetEnabled(bool enabled) { _enabled = enabled; }

protected:
    Object* _owner;
    bool _enabled = true;
};
