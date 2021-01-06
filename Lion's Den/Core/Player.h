#ifndef PLAYER_H
#define PLAYER_H

#include <Core/Audio.h>

#include <Render/Sprite.h>
#include <Render/Animation.h>

#include <IO/Keyboard.h>
#include <IO/Mouse.h>

class Player : public Object
{
    public:
        Player();
        virtual ~Player();

        void OnUpdate();
        void OnRender();
    private:
        Sprite _sprite;

        void MoveIt(const Vec2& offset);

};

#endif // PLAYER_H
