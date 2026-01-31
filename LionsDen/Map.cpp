#include "Map.h"
#include "Tileson/tileson.hpp"
#include "Engine.h"
#include "Render/Drawer.h"
#include "TileCollider.h"
#include "Core/Collider.h"
#include "Dungeon/DungeonTileSet.h"
#include "GUI/Window.h"


float Map::GRID_SCALE;
int Map::SCALED_TILE_SIZE;


Map::Map() {
	Vec2 frame(32, 32);
	_draw_sprite.SetFrameSize(frame);

	Zoom(1);
}
Map::~Map() {

}
void Map::Zoom(float scale) {
	SCALED_TILE_SIZE = 32 * scale;
	GRID_SCALE = 1.0f / SCALED_TILE_SIZE;
}
bool Map::LoadJson(std::string path) {
	_generatedGrid.clear();
	_generatedW = 0;
	_generatedH = 0;

	tson::Tileson parser;
	_map = parser.parse(fs::path(path));
	if (_map->getStatus() == tson::ParseStatus::OK)
	{
		_draw_sprite.SetTexture(Resources::GetTexture("Tilesets/Dungeon_Tileset.png"));
		_draw_sprite.SetFrameSize(Vec2(32, 32));

		SetMapSize(Vec2(_map->getSize().x, _map->getSize().y));
		SetTileSize(Vec2(_map->getTileSize().x, _map->getTileSize().y));
		SetMapSizePixels(Vec2(GetMapSize() * GetTileSize()));

		//_map->getTileset();

		std::cout << "parse ok" << std::endl;
	}
	else //Error occurred
	{
		std::cout << _map->getStatusMessage();
	}


	return _map->getStatus() == tson::ParseStatus::OK;
}

bool Map::LoadFromGenerated(int width, int height, const std::vector<std::vector<int>>& grid) {
	if (width <= 0 || height <= 0 || grid.size() != static_cast<size_t>(height)) return false;
	for (const auto& row : grid) if (row.size() != static_cast<size_t>(width)) return false;

	_map.reset();
	_generatedGrid = grid;
	_generatedW = width;
	_generatedH = height;

	_draw_sprite.SetTexture(Resources::GetTexture("Tilesets/Dungeon_Tileset.png"));
	_draw_sprite.SetFrameSize(Vec2(32, 32));

	SetMapSize(Vec2(static_cast<float>(width), static_cast<float>(height)));
	SetTileSize(Vec2(32, 32));
	SetMapSizePixels(Vec2(GetMapSize() * GetTileSize()));
	return true;
}

bool Map::SaveJson(std::string path)
{
	return false;
}


void Map::Draw() {
	if (!_map) return;

	for (auto& layers : _map->getLayers())
	{
		for (auto& [pos, tileObject] : layers.getTileObjects()) //Loops through absolutely all existing tileObjects
		{
			tson::Tileset* tileset = tileObject.getTile()->getTileset();
			tson::Rect drawingRect = tileObject.getDrawingRect();

			tson::Vector2i position = tileObject.getPositionInTileUnits();
			tson::Tile* tile = tileObject.getTile();
			Vec2 posV2 = Vec2(position.x, position.y);

			_draw_sprite.SetFrame(tile->getId() - 1);
			_draw_sprite.Draw(posV2 * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
		}
	}
}

void Map::DrawLayer(std::string layerName)
{
	if (_map) {
		tson::Layer* layer = _map->getLayer(layerName);
		for (auto& [pos, tileObject] : layer->getTileObjects()) {
			tson::Vector2i position = tileObject.getPositionInTileUnits();
			tson::Tile* tile = tileObject.getTile();
			Vec2 posV2 = Vec2(position.x, position.y);
			_draw_sprite.SetFrame(tile->getId() - 1);
			_draw_sprite.Draw(posV2 * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
		}
		return;
	}
	if (!IsGenerated()) return;
	// Generated: "Floor" / "Floor2" = floor tiles, "Walls" = wall tiles
	const bool drawFloor = (layerName == "Floor" || layerName == "Floor2");
	for (int y = 0; y < _generatedH; ++y) {
		for (int x = 0; x < _generatedW; ++x) {
			int tid = _generatedGrid[static_cast<size_t>(y)][static_cast<size_t>(x)];
			if (drawFloor && tid == DungeonTileSet::FLOOR) {
				_draw_sprite.SetFrame(DungeonTileSet::FLOOR - 1);
				_draw_sprite.Draw(Vec2(static_cast<float>(x), static_cast<float>(y)) * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
			} else if (layerName == "Walls" && tid == DungeonTileSet::WALL) {
				_draw_sprite.SetFrame(DungeonTileSet::WALL - 1);
				_draw_sprite.Draw(Vec2(static_cast<float>(x), static_cast<float>(y)) * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
			}
		}
	}
}

void Map::DrawLayerOrderNR(std::string layerName, int nr)
{
	if (_map) {
		tson::Layer* layer = _map->getLayer(layerName);
		for (auto& [pos, tileObject] : layer->getTileObjects()) {
			tson::Vector2i position = tileObject.getPositionInTileUnits();
			tson::Tile* tile = tileObject.getTile();
			int nrw = tile->get<int>("DrawOrder");
			if (nr == nrw) {
				_draw_sprite.SetFrame(tile->getId() - 1);
				_draw_sprite.Draw(Vec2(position.x, position.y) * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
			}
		}
		return;
	}
	if (IsGenerated() && layerName == "Walls" && nr == 0) {
		for (int y = 0; y < _generatedH; ++y)
			for (int x = 0; x < _generatedW; ++x)
				if (_generatedGrid[static_cast<size_t>(y)][static_cast<size_t>(x)] == DungeonTileSet::WALL) {
					_draw_sprite.SetFrame(DungeonTileSet::WALL - 1);
					_draw_sprite.Draw(Vec2(static_cast<float>(x), static_cast<float>(y)) * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
				}
	}
}



Vec2 Map::GetMapSize()
{
	return MapSize;
}

Vec2 Map::GetTileSize()
{
	return TileSize;
}

Vec2 Map::GetMapSizePixels()
{
	return MapSizePixels;
}

void Map::GetCollisionBoxes()
{
	CollisionTiles.clear();
	if (_map) {
		tson::Tileset* tileset = _map->getTileset("Dungeon_Tileset");
		for (auto& tile : tileset->getTiles()) {
			auto og = tile.getObjectgroup();
			int id = tile.getId();
			for (auto& tileobject : og.getObjects()) {
				Vec2 pos = Vec2(tileobject.getPosition().x, tileobject.getPosition().y);
				Vec2 size = Vec2(tileobject.getSize().x, tileobject.getSize().y);
				CollisionTiles.push_back(TileCollider(pos, size, id));
			}
		}
		return;
	}
	if (IsGenerated()) {
		CollisionTiles.push_back(TileCollider(Vec2(0, 0), Vec2(static_cast<float>(SCALED_TILE_SIZE), static_cast<float>(SCALED_TILE_SIZE)), DungeonTileSet::WALL));
	}
}
void Map::SpawnMonsters()
{
	if (!_map) {
		Monster::MonsterList.clear();
		return;
	}
	tson::Layer* layer = _map->getLayer("Enemies");


	Monster::MonsterList.clear();


	for (auto& [pos, tileObject] : layer->getTileObjects()) //Loops through absolutely all existing tileObjects
	{
		
		tson::Vector2i position = tileObject.getPositionInTileUnits();
		tson::Tile* tile = tileObject.getTile();
		Vec2 posV2 = Vec2(position.x*SCALED_TILE_SIZE, position.y*SCALED_TILE_SIZE);
		//Monster* mon= new Monster(Monster::MonsterTypes[tile->getObjectgroup().getId()]);
		Monster* mon = new Monster();
		mon->SetPos(posV2);
		mon->UpdateId();
		mon->SetParams(Monster::MonsterTypes[tile->getObjectgroup().getId()]);
		Collider::RegisterObject(mon, mon->GetColliderPos(),mon-> GetColliderBox(), false);
		Collider::RegisterTarget(mon, mon->GetColliderPos(),mon-> GetColliderBox());

		Monster::MonsterList.push_back(mon);

	}

}



void Map::RegisterCollisionBoxes() {

	if (_map) {
		for (auto& layers : _map->getLayers()) {
			for (auto& [pos, tileObject] : layers.getTileObjects()) {
				tson::Tile* tile = tileObject.getTile();
				Vec2 pos = Vec2(tileObject.getPosition().x, tileObject.getPosition().y);
				for (auto& col : CollisionTiles) {
					if (col._id == tile->getId()) {
						Object* b = new Object();
						b->SetPos(pos + col._pos);
						b->SetSize(col._size);
						Collider::RegisterObject(b, Collider::COLLIDER_RECT);
						break;
					}
				}
			}
		}
		return;
	}
	if (IsGenerated()) {
		for (int y = 0; y < _generatedH; ++y) {
			for (int x = 0; x < _generatedW; ++x) {
				if (_generatedGrid[static_cast<size_t>(y)][static_cast<size_t>(x)] != DungeonTileSet::WALL) continue;
				for (auto& col : CollisionTiles) {
					if (col._id == DungeonTileSet::WALL) {
						Object* b = new Object();
						b->SetPos(Vec2(static_cast<float>(x * SCALED_TILE_SIZE), static_cast<float>(y * SCALED_TILE_SIZE)) + col._pos);
						b->SetSize(col._size);
						Collider::RegisterObject(b, Collider::COLLIDER_RECT);
						break;
					}
				}
			}
		}
	}
}







void Map::SetMapSize(Vec2 size)
{
	MapSize = size;
}

void Map::SetTileSize(Vec2 size)
{
	TileSize = size;
}

void Map::SetMapSizePixels(Vec2 size)
{
	MapSizePixels = size;
}
