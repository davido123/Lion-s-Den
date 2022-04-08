#include "Map.h"
#include "Tileson/tileson.hpp"
//#include "ImGui/imgui_struct.h"
#include "Engine.h"
#include "Render/Drawer.h"
#include "TileCollider.h"

#include "Core/Collider.h"


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


	return 0;
}

bool Map::SaveJson(std::string path)
{
	return false;
}


void Map::Draw() {

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
	tson::Layer* layer = _map->getLayer(layerName);

	for (auto& [pos, tileObject] : layer->getTileObjects()) //Loops through absolutely all existing tileObjects
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

void Map::DrawLayerOrderNR(std::string layerName, int nr)
{
	tson::Layer* layer = _map->getLayer(layerName);

	for (auto& [pos, tileObject] : layer->getTileObjects()) //Loops through absolutely all existing tileObjects
	{
		tson::Tileset* tileset = tileObject.getTile()->getTileset();
		tson::Rect drawingRect = tileObject.getDrawingRect();

		tson::Vector2i position = tileObject.getPositionInTileUnits();
		tson::Tile* tile = tileObject.getTile();
		Vec2 posV2 = Vec2(position.x, position.y);
		int nrw = tile->get<int>("DrawOrder");

		if (nr == nrw)
		{
			_draw_sprite.SetFrame(tile->getId() - 1);
			_draw_sprite.Draw(posV2 * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
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
	tson::Tileset* tileset = _map->getTileset("Dungeon_Tileset");
	for (auto& tile : tileset->getTiles())
	{
		auto og = tile.getObjectgroup();
		int id = tile.getId();
		for (auto& tileobject : og.getObjects())
		{
			Vec2 pos = Vec2(tileobject.getPosition().x, tileobject.getPosition().y);
			Vec2 size = Vec2(tileobject.getSize().x, tileobject.getSize().y);


			TileCollider col = TileCollider(pos, size, id);

			CollisionTiles.push_back(col);


		}
	}
}
void Map::SpawnMonsters()
{

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

	for (auto& layers : _map->getLayers())
	{

		for (auto& [pos, tileObject] : layers.getTileObjects()) //Loops through absolutely all existing tileObjects
		{
			tson::Tile* tile = tileObject.getTile();

			Vec2 pos = Vec2(tileObject.getPosition().x, tileObject.getPosition().y);
			Vec2 size = Vec2(tile->getTileSize().x, tile->getTileSize().y);



			for (auto& col : CollisionTiles)
			{
				if (col._id == tile->getId())
				{
					Object* b = new Object();
					b->SetPos(pos + col._pos);
					b->SetSize(col._size);
					Collider::RegisterObject(b, Collider::COLLIDER_RECT);

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
