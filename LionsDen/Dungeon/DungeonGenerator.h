#pragma once

#include "DungeonConfig.h"
#include "DungeonTileSet.h"
#include "../Core/Vec2.h"
#include <vector>
#include <cstdint>
#include <random>

/**
 * Room type for dungeon layout
 */
enum class DungeonRoomType {
    Start,   // Safe zone, player spawn, no enemies
    Normal,
    Boss     // One boss, portal (exit) after boss dead
};

/**
 * Room rectangle and type
 */
struct DungeonRoom {
    int x = 0, y = 0, w = 0, h = 0;
    DungeonRoomType type = DungeonRoomType::Normal;

    int CenterX() const { return x + w / 2; }
    int CenterY() const { return y + h / 2; }
};

/**
 * DungeonGenerator - Procedural dungeon layout (rooms + corridors)
 * Deterministic: same config + seed + floor level => same layout
 */
class DungeonGenerator {
public:
    DungeonGenerator() = default;

    /** Generated layout: grid[y][x] = tile ID (DungeonTileSet) */
    using Grid = std::vector<std::vector<int>>;

    /**
     * Generate a dungeon for the given floor level
     * @param floorLevel 1-based floor (1 = first floor, ~5 rooms; 2 = more rooms, etc.)
     * @param config Generation parameters (room count derived from floorLevel)
     * @param outGrid Output grid (cleared and filled)
     * @param outRooms Output room list (Start, Normal, Boss)
     * @return true if generation succeeded
     */
    bool Generate(int floorLevel, const DungeonConfig& config,
                  Grid& outGrid, std::vector<DungeonRoom>& outRooms);

    /** Get player spawn position in tile units (center of Start room) */
    static Vec2 GetStartRoomSpawn(const std::vector<DungeonRoom>& rooms);

private:
    void FillWalls(Grid& grid, int w, int h);
    bool PlaceRooms(const DungeonConfig& config, int roomCount,
                    std::vector<DungeonRoom>& rooms, std::mt19937& rng);
    void CarveRoom(Grid& grid, const DungeonRoom& room);
    void CarveCorridorL(Grid& grid, int x0, int y0, int x1, int y1);
    void MarkStartAndBoss(std::vector<DungeonRoom>& rooms, std::mt19937& rng);
};
