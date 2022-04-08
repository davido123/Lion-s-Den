#ifndef MAP_H
#define	MAP_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <vector>

#include <Core/Resources.h>
#include <Render/Surface.h>
#include <Render/Sprite.h>
#include "Core/Vec2.h"
#include "TileCollider.h"

#include "Tileson/tileson.hpp"
#include "Monster.h"

enum tile {
	TILE_GRASS = 0,
	TILE_WATER,
	TILE_PLAIN_GRASS,
	TILE_TREE,
	TILE_MOUNTAIN,
	TILE_TREE_SPRUCE,
	TILE_OBJECT,
	TILE_BUILDING,
	TILE_GROUND,
	TILE_NONE, // END OF TILES(must be last)
};

class Map {
public:
	//static constexpr int SCALED_TILE_SIZE = 16 * 2;
	//static constexpr float GRID_SCALE = 1.0f / SCALED_TILE_SIZE;
	static int SCALED_TILE_SIZE;
	static float GRID_SCALE;

	static void Zoom(float scale);

	Map();
	virtual ~Map();

	bool LoadJson(std::string path);
	bool SaveJson(std::string path);



	void Draw();
	void DrawLayer(std::string layerName);

	void DrawLayerOrderNR(std::string layerName, int nr);

	Vec2 GetMapSize();
	Vec2 GetTileSize();
	Vec2 GetMapSizePixels();
	void GetCollisionBoxes();

	void SpawnMonsters();

	void RegisterCollisionBoxes();


	void SetMapSize(Vec2 size);
	void SetTileSize(Vec2 size);
	void SetMapSizePixels(Vec2 size);

private:
	std::unique_ptr<tson::Map> _map; //Tileson Map container

	SDL_Texture* _tileset_texture;
	Sprite _draw_sprite;

	Vec2 MapSize;
	Vec2 TileSize;
	Vec2 MapSizePixels;

	std::vector <TileCollider> CollisionTiles;
	

};

#endif	/* MAP_H */

#pragma once
