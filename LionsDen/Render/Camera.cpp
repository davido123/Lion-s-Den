#include "Camera.h"
#include <iostream>
#include <string>


Camera::Camera(const Vec2& pos, const Vec2& viewport) :
        _pos(pos), _viewport(viewport) {
}

void Camera::SetPos(const Vec2& pos) {
    _pos = pos;
    
}
 
void Camera::CenterOnPlayer(const Vec2& playerPos, const Vec2& playerSize, const Vec2& mapSize){

    _pos = Vec2((round(playerPos.x) - W() / 2) + (round(playerSize.x) / 2) + _offset.x,
                (round(playerPos.y) - H() / 2) + (round(playerSize.y) / 2) + _offset.y);

	if (GetPos().x <= 0)
	{
        SetPos(Vec2(0, GetPos().y));
	}
	if (GetPos().y <= 0)
	{
		SetPos(Vec2(GetPos().x, 0));
	}
	if (GetPos().x >= mapSize.x - GetViewport().x)
	{
		SetPos(Vec2(mapSize.x - GetViewport().x, GetPos().y));
	}
	if (GetPos().y >= mapSize.y - GetViewport().y)
	{
		SetPos(Vec2(GetPos().x, mapSize.y - GetViewport().y));
	}




}

void Camera::SetViewport(const Vec2& viewport, const Vec2& offset) {
    _viewport = viewport;
    _offset = offset;
}

bool Camera::InView(SDL_Rect* rect) const {
    if(_pos.x >= rect->x + rect->w || _pos.x + _viewport.x <= rect->x) return false;
    if(_pos.y >= rect->y + rect->h || _pos.y + _viewport.y <= rect->y) return false;
    return true;
}

bool Camera::InView(int x, int y, int h, int w) const {
    if(_pos.x >= x + w || _pos.x + _viewport.x <= x) return false;
    if(_pos.y >= y + h || _pos.y + _viewport.y <= y) return false;
    return true;
}

bool Camera::InView(const Vec2& pos, const Vec2& size) const {
    if(_pos.x >= pos.x + size.x || _pos.x + _viewport.x <= pos.x) return false;
    if(_pos.y >= pos.y + size.y || _pos.y + _viewport.y <= pos.y) return false;
    return true;
}

void Camera::Move(const Vec2& delta_pos) {
    _pos += delta_pos;
}

Vec2 Camera::GetPos() const{
    return _pos;
}

Vec2 Camera::GetViewport() const{
    return _viewport;
}

int Camera::X() const {
    return GetPos().x;
}

int Camera::Y() const {
    return GetPos().y;
}

int Camera::W() const {
    return _viewport.x;
}

int Camera::H() const {
    return _viewport.y;
}

Vec2 Camera::GetOffset()
{
    return _offset;
}

void Camera::SetOffset(Vec2 offset)
{
    _offset = offset;
}

void Camera::SetOffsetX(float x)
{
    _offset.x = x;
}

void Camera::SetOffsetY(float y)
{
    _offset.y = y;
}

