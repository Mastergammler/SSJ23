#include "module.h"

// NOTE: the mouse can only interact with one element at a time
// drag and drop state should go on the ui elements
struct MouseState
{
    int x;
    int y;

    // Is states
    bool left_down;
    bool right_down;

    // event states (this just happened)
    // are only active for one update period
    bool left_clicked;
    bool left_released;
    bool right_clicked;
    bool right_released;
};

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
    TOP = 0b01110000,
    BOTTOM = 0b11100000,
    LEFT = 0b10110000,
    RIGHT = 0b11010000,
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
};

struct SoundCache
{
    WaveBuffer click_hi;
    WaveBuffer click_lo;
    WaveBuffer pop_lo;
    WaveBuffer pop_hi;

    WaveBuffer music;
};

enum UiType
{
    UI_SINGLE = 0x0,
    UI_PANEL = 0x1
};

struct UiElement
{
    bool visible;
    bool initialized;
    bool hovered;

    int id;

    int x_start;
    int y_start;
    int x_end;
    int y_end;

    int x_tiles;
    int y_tiles;

    int layer;
    // FIXME: this is not pretty how to do this better?
    int sprite_index;
    int hover_sprite_index;

    UiType type;
    Action on_click;
};

struct UiElementStorage
{
    UiElement* elements;

    int count;
    int size;
    int layer_count;
};

struct UiState
{
    bool show_crafting_panel;
    bool tower_placement_mode;
    bool tower_a_selected;
    bool ui_focus;

    int crafting_panel_id;
};

struct Sprite
{
    int x_tiles;
    int y_tiles;
    int sheet_start_index;

    SpriteSheet* sheet;
};

struct SpriteCache
{
    Sprite tower_a;
    Sprite tower_b;
};

/**
 * If the boundary should count as another some tile there or not
 */
const bool BOUNDARY_DEFAULT = true;

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

    int tile_id;
    int is_start;
    int is_end;

    /**
     * Bit-Order T L R B TL TR BL BR
     */
    BYTE adjacent;
};

struct TileMap
{
    int rows;
    int columns;
    int tile_count;

    TileSize tile_size;

    /**
     * Tiles with starting position UL
     * The renderer has to take this into consideration
     */
    Tile* tiles;

    bool sameHorizontally(int tileIdx, int otherIdx)
    {
        // of not same row, means we're at either boundary
        if (tileRowOf(tileIdx) == tileRowOf(otherIdx))
        {
            return areSame(tileIdx, otherIdx);
        }

        // Boundry case
        return BOUNDARY_DEFAULT;
    }

    bool sameVertically(int tileIdx, int otherIdx)
    {
        int tileRow = tileRowOf(tileIdx);
        int otherRow = tileRowOf(otherIdx);

        if (tileRow == 0 && otherRow < tileRow) return BOUNDARY_DEFAULT;
        if (tileRow == rows - 1 && otherRow > tileRow) return BOUNDARY_DEFAULT;

        return areSame(tileIdx, otherIdx);
    }

    // difference - row has to be different!
    // but only difference of one
    bool areSameAllDir(int tileIdx, int otherIdx)
    {
        int diff = tileRowOf(tileIdx) - tileRowOf(otherIdx);
        int normalized = abs(diff);

        // it has to be one row above or below the current row
        if (normalized != 1) return BOUNDARY_DEFAULT;
        return sameVertically(tileIdx, otherIdx);
    }

    bool areSame(int tileIdx, int otherIdx)
    {
        if (otherIdx < 0) return BOUNDARY_DEFAULT;
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

        if (yBottomUp) { tileIdx = MirrorIndex(tileIdx, columns, rows); }

        return tiles[tileIdx].tile_id;
    }

    Tile* tileAt(int x, int y, bool yBottomUp = true)
    {
        int tileX = x / tile_size.width;
        int tileY = y / tile_size.height;

        int tileIdx = tileY * columns + tileX;
        if (yBottomUp) { tileIdx = MirrorIndex(tileIdx, columns, rows); }

        // Tile* tile = &tiles[tileIdx];
        // Tile* copy = tile;

        return tiles + tileIdx;
    }
};
