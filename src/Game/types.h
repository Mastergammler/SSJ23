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

struct Tile
{
    int width;
    int height;
};

struct Button
{
    int x_start;
    int y_start;
    int x_end;
    int y_end;

    int default_sheet_start_idx;
    int hover_sheet_start_idx;
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
};

struct InteractionState
{
    bool show_crafting_panel;
    bool show_tower_placement;
    bool ui_focus;
};
