/*
 * Animation.cpp
 *
 *  Created on: 22 июля 2014 г.
 *      Author: snickers
 */
#include <iostream>
#include "Animation.h"

Animation::Animation() :
	_begin_frame(0),
	_max_frame(0),
	_current_frame(0),
	_frame_rate(100),
	_frame_inc(1),
	_old_time(0)

{
	_animation_finished = false;
	_animate_once = false;
	//constructor
}

void Animation::SetCurrentFrame(Uint16 frame) {//temporary modified
	/*
	if (frame >= _begin_frame && frame <= _max_frame) {
		_current_frame = frame;
	}else{
		_current_frame = _begin_frame;
	}
	*/

	_current_frame = frame;


}

void Animation::SetFrameRate(Uint16 rate) {
	if (rate >= 0) {
		_frame_rate = rate;
	}
}

void Animation::SetBeginFrame(Uint16 frame) {
	_begin_frame = frame;
}

void Animation::SetMaxFrame(Uint16 frame) {
	_max_frame = frame;
}

void Animation::SetFrameSize(const Vec2& size) {
	_frame_size = size;
}

void Animation::SetSound(Audio* sound)
{
	_animation_sound = sound;
	_have_animation_sound = true;
}

Uint16 Animation::GetBeginFrame() const {
	return _begin_frame;
}

Uint16 Animation::GetMaxFrame() const {
	return _max_frame;
}

Uint16 Animation::GetCurrentFrame() const {
	return _current_frame;
}

Uint16 Animation::GetFrameRate() const {
	return _frame_rate;
}

Vec2 Animation::GetFrameSize() const {
	return _frame_size;
}

bool Animation::IsAnimationFinished()
{
	return _animation_finished;
}

void Animation::SetAnimationFinished(bool f)
{
	_animation_finished = f;
}

void Animation::ResetFrame()
{
	_current_frame = _begin_frame;
}

void Animation::OnAnimation() {
	/**/
	if (_have_animation_sound)
	{
		if (_current_frame == _begin_frame+1 && !_animation_sound_played)
		{
			_animation_sound->Play();
			_animation_sound_played = true;
		}

		

	}
	if (_animate_once)
	{

		if (_old_time + _frame_rate > SDL_GetTicks() || _begin_frame == _max_frame || _animation_finished == true) {
			//_animation_finished=false;
			return;
		}

		_old_time = SDL_GetTicks();

		_current_frame += _frame_inc;

		if (_current_frame > _max_frame) {
			_current_frame = _begin_frame;
			_animation_finished = true;
			_animation_sound_played = false;
		}

	}
	else
	{
		if (_old_time + _frame_rate > SDL_GetTicks() || _begin_frame == _max_frame) {
			return;
		}

		_old_time = SDL_GetTicks();

		_current_frame += _frame_inc;

		if (_current_frame > _max_frame) {
			_current_frame = _begin_frame;
			_animation_sound_played = false;
		}

	}



}
