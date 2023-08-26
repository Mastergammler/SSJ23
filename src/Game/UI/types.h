#include "../module.h"

enum UiType
{
    UI_BUTTON = 0x0,
    UI_PANEL = 0x1,
    UI_DRAG_DROP = 0x2
};

enum UiFlag
{
    DRAG_SOURCE = 0b1 << 0,
    DROP_TARGET = 0b1 << 1
};

enum UiPosition
{
    UPPER_LEFT,
    UPPER_MIDDLE,
    UPPER_RIGHT,
    CENTERED
};

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

struct UiSprite : Sprite
{
    UiSprite(int xTiles,
             int yTiles,
             int spriteIndex,
             int hoverIndex,
             SpriteSheet* sheet)
    {
        this->x_tiles = xTiles;
        this->y_tiles = yTiles;
        this->sheet_start_index = spriteIndex;
        this->hover_start_index = hoverIndex;
        this->sheet = sheet;
    }

    int hover_start_index;
};

struct Position
{
    int x, y;
};

struct UniformGrid
{
    int rows;
    int columns;

    int items;

    Position* item_draw_positions;
};

struct Anchor
{
    UiPosition reference_point;

    /**
     * x offset in tiles
     */
    float x_offset;

    /**
     * y offset in tiles
     */
    float y_offset;
};

struct UiElement
{
    bool visible;
    bool initialized;
    bool hovered;

    int id;
    /**
     * Id of the parent UI element, or -1 if none
     */
    int parent_id;

    int flags;

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

//-------------
//  UI STATE
//-------------

struct Navigation
{
    bool in_menu;
    bool in_start_screen;
    bool in_game;
};

struct EntitySlotMap
{
    int current_slot_id;
    int entity_id;

    /**
     * for resetting
     */
    int initial_slot_id;
};

struct MenuElements
{
    int start_game_button;
    int map_selection_panel;
    int exit_game_button;
};

struct CraftingElements
{
    bool visible;

    int show_hide_button;
    int tower_panel;
    int items_panel;

    IntArray item_slots;
    IntArray tower_slots;

    /**
     * count of available items
     * for each item there has to exist a entity slot entry
     * ! But there can be more item slots than items !
     */
    int item_count;
    EntitySlotMap* slot_map;
};

struct PlacementElements
{
    bool active;
    int tower_selection_panel;

    // tmp
    bool tower_a_selected;
};

struct UiState
{
    bool ui_focus;
    bool is_dragging;
    UiElement* ui_focus_element;

    MenuElements menu = {};
    CraftingElements crafting = {};
    PlacementElements placement = {};

    // TODO: this seems wrong here, do i really need it?
    int dragged_entity_id;

    // tmp
    int tmp_2;
};
