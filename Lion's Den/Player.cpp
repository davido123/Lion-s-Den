#include "Player.h"

Player::Player()
{
    Vec2 sprite_size(64, 64);
    SetSize(sprite_size);
	_side = SIDE_NONE;
    _speed=100;

	//stay
	_anim_stay.SetBeginFrame(20);
	_anim_stay.SetMaxFrame(20);
	_anim_stay.SetCurrentFrame(20);

	//Running up side
	_anim_run_up.SetBeginFrame(0);
	_anim_run_up.SetMaxFrame(8);
	_anim_run_up.SetCurrentFrame(0);

	//Running left side
	_anim_run_left.SetBeginFrame(9);
	_anim_run_left.SetMaxFrame(17);
	_anim_run_left.SetCurrentFrame(9);

	//Running down side
	_anim_run_down.SetBeginFrame(18);
	_anim_run_down.SetMaxFrame(26);
	_anim_run_down.SetCurrentFrame(18);

	//Running right side
	_anim_run_right.SetBeginFrame(27);
	_anim_run_right.SetMaxFrame(35);
	_anim_run_right.SetCurrentFrame(27);

	_sprite.SetTexture(Resources::GetTexture("male_walkcycle.png"));
	_sprite.SetAnimation(_anim_stay);
	_sprite.SetFrameSize(sprite_size);
	_sprite.SetAnimationRate(300);








    //ctor
}

Player::~Player()
{
    //dtor
}

void Player::OnRender() {
    _sprite.Draw(GetGlobalPos(), Vec2(64, 64), Window::GetCamera());
}
void Player::OnUpdate() {

        if (Keyboard::isKeyDown(KEY_RIGHT)) {Turn(SIDE_RIGHT);}
        else if (Keyboard::isKeyDown(KEY_LEFT)) {Turn(SIDE_LEFT);}
        else if (Keyboard::isKeyDown(KEY_UP)) {Turn(SIDE_UP);}
        else if (Keyboard::isKeyDown(KEY_DOWN)) {Turn(SIDE_DOWN);}
        //else if (Keyboard::isKeyDown(KEY_SPACE)) {MoveRandom(rand()%4+1);}
        else {
            Turn(SIDE_NONE);
        }





    //MoveRandom();
}

void Player::Turn(turn_side side) {
    switch (side) {
        case SIDE_RIGHT:
			if (_side != side) _sprite.SetAnimation(_anim_run_right);
            Move(Vec2(+2.0f, 0.0f));
            break;

        case SIDE_LEFT:
			if (_side != side) _sprite.SetAnimation(_anim_run_left);
            Move(Vec2(-2.0f, 0.0f));
            break;

        case SIDE_UP:
			if (_side != side) _sprite.SetAnimation(_anim_run_up);
            Move(Vec2(0.0f, -2.0f));
            break;

        case SIDE_DOWN:
			if (_side != side) _sprite.SetAnimation(_anim_run_down);
            Move(Vec2(0.0f, +2.0f));
            break;

        case SIDE_NONE:
			_sprite.SetAnimation(_anim_stay);

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
