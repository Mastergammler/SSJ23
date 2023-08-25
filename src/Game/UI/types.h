#include "../module.h"

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

enum UiType
{
    UI_SINGLE = 0x0,
    UI_PANEL = 0x1
};

enum UiPosition
{
    UPPER_LEFT,
    UPPER_MIDDLE,
    UPPER_RIGHT,
    CENTERED
};

struct Position
{
    int x, y;
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

struct ItemPanelPositionMap
{
    int current_slot_id;
    int entity_id;
    /**
     * for resetting
     */
    int initial_slot_id;
};

struct UiState
{
    bool show_crafting_panels;
    bool tower_placement_mode;
    bool tower_a_selected;
    bool ui_focus;

    int tower_crafting_panel_id;
    int items_panel_id;
    int tower_selection_panel_id;

    int tmp_1;
    int tmp_2;

    // TODO: don't like this saving here
    //  this needs to be done another way?
    int start_game_button_id;
    int map_selection_panel_id;
    int exit_game_button_id;

    map<int, ItemPanelPositionMap> ui_entity_map = {};
    IntArray item_slots;
};

struct Navigation
{
    bool in_menu;
    bool in_start_screen;
    bool in_game;
};
