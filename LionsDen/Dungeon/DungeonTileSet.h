#pragma once

/**
 * DungeonTileSet - Tile ID constants for generated dungeons
 * Match your Dungeon_Tileset (Tiled) tile IDs so Map rendering works
 * Tiled uses 1-based tile IDs; adjust if your tileset differs
 */
namespace DungeonTileSet {
    const int EMPTY = 0;
    const int FLOOR = 1;
    const int WALL = 2;
    const int DOOR = 3;
    const int PORTAL = 4;  // Exit to next level (in boss room)
    // Add more as needed for your tileset
}
