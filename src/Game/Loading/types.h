#include "../module.h"

struct TileSize
{
    int width;
    int height;
};

/**
 * Maps the adjacency state bit orders to a enum
 * Bit order: T L R B TL TR BL BR
 */
enum TileEnv
{
    SINGLE = 0b00000000,
    MIDDLE = 0b11111111,
    HORIZONTAL = 0b0110 << 4,
    VERTICAL = 0b1001 << 4,
    TOP = 0b01110000,
    TOP_END = 0b0001 << 4,
    BOTTOM = 0b11100000,
    BOTTOM_END = 0b1000 << 4,
    LEFT = 0b10110000,
    LEFT_END = 0b0010 << 4,
    RIGHT = 0b11010000,
    RIGHT_END = 0b0100 << 4,
    TL = 0b00110000,
    TR = 0b01010000,
    BL = 0b10100000,
    BR = 0b11000000,
    ITL = 0b11111110,
    ITR = 0b11111101,
    IBL = 0b11111011,
    IBR = 0b11110111
};

struct BitmapCache
{
    BitmapBuffer cursor_sprite = {};
    BitmapBuffer tile_highlight = {};

    SpriteSheet ground = {};
    SpriteSheet ui = {};
    SpriteSheet characters = {};
    SpriteSheet items = {};
};

struct SoundCache
{
    WaveBuffer click_hi;
    WaveBuffer click_lo;
    WaveBuffer pop_lo;
    WaveBuffer pop_hi;
    WaveBuffer craft_error;
    WaveBuffer craft_success;

    WaveBuffer music;
};

struct SpriteCache
{
    Sprite tower_a;
    Sprite tower_b;
    Sprite enemy_a;
};

struct SpriteAnimation
{
    int sprite_count;
    float time_per_sprite;
    Sprite* sprites;
};

struct AnimationCache
{
    SpriteAnimation enemy_anim;
};

/**
 * If the boundary should count as another some tile there or not
 */
const bool BOUNDARY_DEFAULT = false;

enum TileType
{
    GRASS_TILE = 0,
    PATH_TILE = 1
};

struct Tile
{
    bool is_occupied;

    /**
     * Tilemap x pos
     * NOT a pixel position
     */
    int x;

    /**
     * TileMap y pos - anchor TOP
     * NOT a pixel position
     */
    int y;

    TileType tile_id;
    int is_start;
    int is_end;

    /**
     * Bit-Order T L R B TL TR BL BR
     */
    u8 adjacent;
};

struct TileMap
{
    int rows;
    int columns;
    int tile_count;

    int spawn_count;
    int target_count;

    TileSize tile_size;

    /**
     * Tiles with starting position UL
     * The renderer has to take this into consideration
     */
    Tile* tiles;
    Tile** targets;
    Tile** spawns;

    bool boundaryDefault(int tileIndex)
    {
        if (tiles[tileIndex].tile_id == GRASS_TILE)
        {
            return true;
        }
        return BOUNDARY_DEFAULT;
    }

    bool sameHorizontally(int tileIdx, int otherIdx)
    {
        // of not same row, means we're at either boundary
        if (tileRowOf(tileIdx) == tileRowOf(otherIdx))
        {
            return areSame(tileIdx, otherIdx);
        }

        // Boundry case
        return boundaryDefault(tileIdx);
    }

    bool sameVertically(int tileIdx, int otherIdx)
    {
        int tileRow = tileRowOf(tileIdx);
        int otherRow = tileRowOf(otherIdx);

        if (tileRow == 0 && otherRow < tileRow) return boundaryDefault(tileIdx);
        if (tileRow == rows - 1 && otherRow > tileRow)
            return boundaryDefault(tileIdx);

        return areSame(tileIdx, otherIdx);
    }

    // difference - row has to be different!
    // but only difference of one
    bool areSameAllDir(int tileIdx, int otherIdx)
    {
        int diff = tileRowOf(tileIdx) - tileRowOf(otherIdx);
        int normalized = abs(diff);

        // it has to be one row above or below the current row
        if (normalized != 1) return boundaryDefault(tileIdx);
        return sameVertically(tileIdx, otherIdx);
    }

    bool areSame(int tileIdx, int otherIdx)
    {
        if (otherIdx < 0) return boundaryDefault(tileIdx);
        assert(tileIdx >= 0 && "Tile index must be grater than 0");

        int thisTileId = tiles[tileIdx].tile_id;
        int otherTileId = tiles[otherIdx].tile_id;

        return thisTileId == otherTileId;
    }

    int tileRowOf(int idx)
    {
        if (idx < 0) return -1;

        return idx / columns;
    }

    int tileIdOf(int x, int y, bool yBottomUp = true)
    {
        int tileX = x / tile_size.width;
        int tileY = y / tile_size.height;

        int tileIdx = tileY * columns + tileX;

        if (yBottomUp)
        {
            tileIdx = MirrorIndex(tileIdx, columns, rows);
        }

        return tiles[tileIdx].tile_id;
    }

    Tile* tileAt(int x, int y, bool yBottomUp = true)
    {
        int tileX = x / tile_size.width;
        int tileY = y / tile_size.height;

        int tileIdx = tileY * columns + tileX;
        if (yBottomUp)
        {
            tileIdx = MirrorIndex(tileIdx, columns, rows);
        }

        // Tile* tile = &tiles[tileIdx];
        // Tile* copy = tile;

        return tiles + tileIdx;
    }
};

struct Resources
{
    BitmapCache bitmaps;
    SoundCache audio;
    SpriteCache sprites;
    AnimationCache animations;
    ItemArray items;
};
