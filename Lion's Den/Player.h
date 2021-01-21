#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>

#include <Core/Object.h>
#include <Core/Vec2.h>
#include <Core/Timer.h>
#include <Core/Resources.h>
#include <Core/Audio.h>

#include <Render/Sprite.h>
#include <Render/Animation.h>

#include <IO/Keyboard.h>
#include <IO/Mouse.h>
enum turn_side {
	SIDE_NONE = 0, // staying
	SIDE_UP,
	SIDE_DOWN,
	SIDE_LEFT,
	SIDE_RIGHT
};

class Player : public Object {
public:
	Player();
	virtual ~Player();

	void OnUpdate();
	void OnRender();
	unsigned int _speed; //keyboard delay, lower is faster
	void Turn(turn_side side);
	void MoveRandom(int r);
private:








	Timer _input_timer;
	Sprite _sprite;
	turn_side _side;

	Animation _anim_stay;
	Animation _anim_run_up;
	Animation _anim_run_down;
	Animation _anim_run_left;
	Animation _anim_run_right;






};

#endif // PLAYER_H
