#include "Player.h"

Player::Player()
{
    Vec2 sprite_size(16, 16);
    SetSize(sprite_size);
    _sprite.SetTexture(Resources::GetTexture("test.png"));
    //_sprite.SetAnimation(_anim_run_left);
    _sprite.SetFrameSize(sprite_size);
    //_sprite.SetAnimationRate(300);




    //ctor
}

Player::~Player()
{
    //dtor
}


void Player::OnUpdate(){




}

void Player::OnRender(){


   //_sprite.Draw(GetGlobalPos() , GetSize(), Window::GetCamera());


}
