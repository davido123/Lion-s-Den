#include "Map.h"
#include "Core/Vec2.h"

float Map::GRID_SCALE;
int Map::SCALED_TILE_SIZE;

Map::Map() {
	_max_grid_x = 0;
	_max_grid_y = 0;
	_tile_grid_size = 0;
	_Tile_map = nullptr;
	
	
	Vec2 frame(32, 32);

	//setup ground tile
	_sprite_ground.SetTexture(Resources::GetTexture("Dungeon_Tileset.png"));
	_sprite_ground.SetFrameSize(frame);
	_sprite_ground.SetFrame(89);



	_nosprite.SetFrameSize(frame);

	Zoom(2);
}

Map::Map(const Map& orig) {

}

Map::~Map() {

}

void Map::Zoom(float scale) {
	SCALED_TILE_SIZE = 16 * scale;
	GRID_SCALE = 1.0f / SCALED_TILE_SIZE;
}

bool Map::Load(std::string path) {
	std::fstream file_map(path);

	size_t width;
	size_t height;
	file_map >> seed;
	file_map >> width;
	file_map >> height;

	if (width != 0 && height != 0) {
		_max_grid_x = width;
		_max_grid_y = height;

		std::cout << "Map size: w " << width << ", h " << height << std::endl;
	}
	else {
		std::cerr << "Invalid sizes: " << width << ", " << height << std::endl;
		return false;
	}

	//Init map array
	_Tile_map = new tile*[_max_grid_y];
	for (size_t i = 0; i < _max_grid_y; ++i) {
		_Tile_map[i] = new tile[_max_grid_x];
	}

	std::string line;
	size_t words_count;
	size_t lines_count = 0;

	while (getline(file_map, line) && lines_count < _max_grid_y) {

		//skip null line
		if (line.empty()) {
			continue;
		}

		words_count = 0;

		std::stringstream line_stream(line);
		size_t ntile;

		while (line_stream >> ntile && words_count < _max_grid_x) {
			if (ntile > TILE_NONE) {
				std::cerr << "Wrong tile: " << ntile << std::endl;
				return false;
			}

			_Tile_map[lines_count][words_count] = static_cast<tile> (ntile);
			++words_count;
		}

		if (words_count != _max_grid_x) {
			std::cerr << "Wrong map size: " << words_count << ", expected " << _max_grid_x << std::endl;
			return false;
		}

		++lines_count;
	}

	file_map.close();

	return true;
}

bool Map::SaveMap(std::string path) {
	std::ofstream file_map(path, std::ios::trunc);

	file_map << _max_grid_x << " " << _max_grid_y << std::endl;

	for (size_t i = 0; i < _max_grid_y; ++i) {
		for (size_t j = 0; j < _max_grid_x; ++j) {
			file_map << _Tile_map[i][j] << " ";
		}
		file_map << std::endl;
	}

	file_map.close();

	return true;
}

void Map::DrawTile(const Vec2& pos_local, tile mtile) {

	tile left = GetTile(pos_local + Vec2::LEFT);
	tile right = GetTile(pos_local + Vec2::RIGHT);
	tile up = GetTile(pos_local + Vec2::UP);
	tile down = GetTile(pos_local + Vec2::DOWN);

	Sprite* draw_sprite;
	switch (mtile) {

		break;
	case TILE_GROUND:
		draw_sprite = &_sprite_ground;
		_sprite_ground.SetFrame(89);

		break;

	case TILE_OBJECT:
	default:
		draw_sprite = &_nosprite;
	}

	draw_sprite->Draw(pos_local * SCALED_TILE_SIZE, Vec2(SCALED_TILE_SIZE, SCALED_TILE_SIZE), Window::GetCamera());
}

void Map::Draw() {
	Vec2 local_pos;
	for (size_t i = 0; i < _max_grid_y; ++i) {
		for (size_t j = 0; j < _max_grid_x; ++j) {
			local_pos.x = j;
			local_pos.y = i;
			DrawTile(local_pos, _Tile_map[i][j]);
		}
	}
}

void Map::DrawDebugTileRect(const Vec2& pos, tile mtile) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= _max_grid_x || pos.y >= _max_grid_y) {
		return;
	}

	SDL_Rect tile_border_pos = { (int)pos.x*SCALED_TILE_SIZE - Window::GetCamera()->X() - 1,
							(int)pos.y*SCALED_TILE_SIZE - Window::GetCamera()->Y() - 1,
							SCALED_TILE_SIZE + 2, SCALED_TILE_SIZE + 2 };
	Surface::DrawRect(&tile_border_pos, COLOR_YELLOW);

	DrawTile(Vec2((int)pos.x, (int)pos.y), mtile);
}

void Map::Free() {
	for (size_t i = 0; i < _max_grid_x; ++i) {

		delete[](_Tile_map[i]);
	}
	delete[](_Tile_map);

	_max_grid_x = 0;
	_max_grid_y = 0;
	_tile_grid_size = 0;
	_Tile_map = nullptr;
}

bool Map::CanMove(const Vec2& pos) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= _max_grid_x || pos.y >= _max_grid_y) {
		return false;
	}

	tile mtile = _Tile_map[static_cast<int> (pos.y)][static_cast<int> (pos.x)];
	if (mtile == TILE_WATER || mtile == TILE_MOUNTAIN || mtile == TILE_BUILDING) {
		return false;
	}

	return true;
}

bool Map::CanMove(tile mtile) {
	switch (mtile) {
	case TILE_WATER:
	case TILE_MOUNTAIN:
	case TILE_BUILDING:
		return false;
	default:
		return true;
	}
}

bool Map::SetTile(const Vec2& pos, tile mtile) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= _max_grid_x || pos.y >= _max_grid_y) {
		return false;
	}

	_Tile_map[static_cast<int> (pos.y)][static_cast<int> (pos.x)] = mtile;

	return true;
}


tile Map::GetTileNear(const Vec2& pos, const Vec2& side_offset) {
	Vec2 next(pos.x + side_offset.x, pos.y + side_offset.y);
	return GetTile(next);
}

size_t Map::GetW() {
	return _max_grid_x;
}

size_t Map::GetH() {
	return _max_grid_y;
}
