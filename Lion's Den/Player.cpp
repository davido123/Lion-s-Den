#include "Player.h"

Player::Player()
{
    Vec2 sprite_size(16, 16);
    SetSize(sprite_size);
    _sprite.SetTexture(Resources::GetTexture("test.png"));
    _speed=100;


    //ctor
}

Player::~Player()
{
    //dtor
}

void Player::OnRender() {
    _sprite.Draw(GetGlobalPos(), Vec2(32, 32), Window::GetCamera());
}
void Player::OnUpdate() {

        if (Keyboard::isKeyDown(KEY_RIGHT)) {Turn(SIDE_RIGHT);}
        else if (Keyboard::isKeyDown(KEY_LEFT)) {Turn(SIDE_LEFT);}
        else if (Keyboard::isKeyDown(KEY_UP)) {Turn(SIDE_UP);}
        else if (Keyboard::isKeyDown(KEY_DOWN)) {Turn(SIDE_DOWN);}
        else if (Keyboard::isKeyDown(KEY_SPACE)) {MoveRandom(rand()%4+1);}
        else {
            Turn(SIDE_NONE);
        }





    //MoveRandom();
}

void Player::Turn(turn_side side) {
    switch (side) {
        case SIDE_RIGHT:
            Move(Vec2(+1.0f, 0.0f));
            break;

        case SIDE_LEFT:
            Move(Vec2(-1.0f, 0.0f));
            break;

        case SIDE_UP:
            Move(Vec2(0.0f, -1.0f));
            break;

        case SIDE_DOWN:
            Move(Vec2(0.0f, +1.0f));
            break;

        case SIDE_NONE:
            break;
    }

}
void Player::MoveRandom(int r)
{
    if(r==1&& GetGlobalPos().x <= Window::GetWidth())
        Move(Vec2(+1.0f, 0.0f));//r
    else if(r==2&&GetGlobalPos().x >= 0)
        Move(Vec2(-1.0f, 0.0f));//l
    else if(r==3&&GetGlobalPos().y >= 0)
        Move(Vec2(0.0f, -1.0f));//up
    else if(r==4&&GetGlobalPos().y <= Window::GetHeight())
        Move(Vec2(0.0f, +1.0f));//down




}
