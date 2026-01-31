#include "DungeonGenerator.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
    bool RoomsOverlap(const DungeonRoom& a, const DungeonRoom& b, int margin = 1) {
        return a.x + a.w + margin > b.x && b.x + b.w + margin > a.x &&
               a.y + a.h + margin > b.y && b.y + b.h + margin > a.y;
    }
}

bool DungeonGenerator::Generate(int floorLevel, const DungeonConfig& config,
                                Grid& outGrid, std::vector<DungeonRoom>& outRooms) {
    outGrid.clear();
    outRooms.clear();

    int w = config.gridWidth;
    int h = config.gridHeight;
    if (w < 10 || h < 10) return false;

    int roomCount = config.GetRoomCountForFloor(floorLevel);
    roomCount = std::max(2, std::min(roomCount, 20));  // At least Start + Boss

    outGrid.resize(static_cast<size_t>(h), std::vector<int>(static_cast<size_t>(w), DungeonTileSet::WALL));

    std::mt19937 rng(config.seed != 0 ? static_cast<std::mt19937::result_type>(config.seed)
                                       : static_cast<std::mt19937::result_type>(std::random_device{}()));

    if (!PlaceRooms(config, roomCount, outRooms, rng)) {
        std::cerr << "DungeonGenerator: Failed to place rooms" << std::endl;
        return false;
    }

    MarkStartAndBoss(outRooms, rng);

    for (const auto& room : outRooms)
        CarveRoom(outGrid, room);

    // Connect rooms with L-shaped corridors (simple: connect consecutive by index)
    for (size_t i = 1; i < outRooms.size(); ++i) {
        int x0 = outRooms[i - 1].CenterX();
        int y0 = outRooms[i - 1].CenterY();
        int x1 = outRooms[i].CenterX();
        int y1 = outRooms[i].CenterY();
        CarveCorridorL(outGrid, x0, y0, x1, y1);
    }

    return true;
}

void DungeonGenerator::FillWalls(Grid& grid, int w, int h) {
    grid.resize(static_cast<size_t>(h), std::vector<int>(static_cast<size_t>(w), DungeonTileSet::WALL));
}

bool DungeonGenerator::PlaceRooms(const DungeonConfig& config, int roomCount,
                                   std::vector<DungeonRoom>& rooms, std::mt19937& rng) {
    std::uniform_int_distribution<int> distW(config.minRoomWidth, config.maxRoomWidth);
    std::uniform_int_distribution<int> distH(config.minRoomHeight, config.maxRoomHeight);
    std::uniform_int_distribution<int> distX(1, config.gridWidth - config.maxRoomWidth - 1);
    std::uniform_int_distribution<int> distY(1, config.gridHeight - config.maxRoomHeight - 1);

    int attempts = roomCount * 50;
    while (static_cast<int>(rooms.size()) < roomCount && attempts-- > 0) {
        DungeonRoom r;
        r.w = distW(rng);
        r.h = distH(rng);
        r.x = distX(rng);
        r.y = distY(rng);

        bool overlap = false;
        for (const auto& other : rooms) {
            if (RoomsOverlap(r, other)) {
                overlap = true;
                break;
            }
        }
        if (!overlap)
            rooms.push_back(r);
    }
    return static_cast<int>(rooms.size()) >= 2;
}

void DungeonGenerator::CarveRoom(Grid& grid, const DungeonRoom& room) {
    int h = static_cast<int>(grid.size());
    int w = h > 0 ? static_cast<int>(grid[0].size()) : 0;
    for (int dy = 0; dy < room.h; ++dy) {
        int gy = room.y + dy;
        if (gy < 0 || gy >= h) continue;
        for (int dx = 0; dx < room.w; ++dx) {
            int gx = room.x + dx;
            if (gx < 0 || gx >= w) continue;
            grid[static_cast<size_t>(gy)][static_cast<size_t>(gx)] = DungeonTileSet::FLOOR;
        }
    }
}

void DungeonGenerator::CarveCorridorL(Grid& grid, int x0, int y0, int x1, int y1) {
    int h = static_cast<int>(grid.size());
    int w = h > 0 ? static_cast<int>(grid[0].size()) : 0;
    int x = x0, y = y0;
    while (x != x1) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            grid[static_cast<size_t>(y)][static_cast<size_t>(x)] = DungeonTileSet::FLOOR;
        x += (x1 > x0 ? 1 : -1);
    }
    while (y != y1) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            grid[static_cast<size_t>(y)][static_cast<size_t>(x)] = DungeonTileSet::FLOOR;
        y += (y1 > y0 ? 1 : -1);
    }
    if (x >= 0 && x < w && y >= 0 && y < h)
        grid[static_cast<size_t>(y)][static_cast<size_t>(x)] = DungeonTileSet::FLOOR;
}

void DungeonGenerator::MarkStartAndBoss(std::vector<DungeonRoom>& rooms, std::mt19937& rng) {
    if (rooms.size() < 2) return;
    std::uniform_int_distribution<size_t> dist(0, rooms.size() - 1);
    size_t startIdx = dist(rng);
    size_t bossIdx = dist(rng);
    while (bossIdx == startIdx && rooms.size() > 1)
        bossIdx = dist(rng);
    rooms[startIdx].type = DungeonRoomType::Start;
    rooms[bossIdx].type = DungeonRoomType::Boss;
}

Vec2 DungeonGenerator::GetStartRoomSpawn(const std::vector<DungeonRoom>& rooms) {
    for (const auto& r : rooms) {
        if (r.type == DungeonRoomType::Start) {
            float cx = static_cast<float>(r.CenterX());
            float cy = static_cast<float>(r.CenterY());
            return Vec2(cx, cy);
        }
    }
    return Vec2(0.f, 0.f);
}
