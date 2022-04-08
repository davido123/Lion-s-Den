#pragma once
#include <vector>
#include "Core/Vec2.h"

class TileCollider {
public:
	TileCollider(Vec2 pos, Vec2 size, int id);

	Vec2 _pos;
	Vec2 _size;
	int _id;
};