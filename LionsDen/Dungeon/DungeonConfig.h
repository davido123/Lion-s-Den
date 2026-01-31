#pragma once

#include <cstdint>

/**
 * DungeonConfig - Parameters for procedural dungeon generation
 * Room count depends on floor level: first floor ~baseRooms, each next floor has more
 */
struct DungeonConfig {
    int gridWidth = 80;
    int gridHeight = 60;
    int baseRoomCount = 5;       // First floor room count; floor N gets baseRoomCount + (N-1) rooms
    int minRoomWidth = 5;
    int minRoomHeight = 5;
    int maxRoomWidth = 12;
    int maxRoomHeight = 10;
    uint32_t seed = 0;          // 0 = use random seed each time

    /** Room count for a given floor level (1-based). First floor ~5, each next floor more. */
    int GetRoomCountForFloor(int floorLevel) const {
        if (floorLevel < 1) floorLevel = 1;
        return baseRoomCount + (floorLevel - 1);
    }
};
