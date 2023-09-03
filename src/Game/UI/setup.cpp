#include "../internal.h"
#include "../ui.h"

// test test test
UiElementStorage uiElements;

// TODO: still has a few hard coded initializers
int CreateButton(Anchor anchor, UiSprite sprite, Action onClick, int layer = 0)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];

    button->id = id;
    button->initialized = true;

    button->x_tiles = sprite.x_tiles;
    button->y_tiles = sprite.y_tiles;
    button->sprite_index = sprite.sheet_start_index;
    button->hover_sprite_index = sprite.hover_start_index;

    Position start = CalculateStartPixelPosition(anchor, sprite);

    button->x_start = start.x;
    button->y_start = start.y;
    button->x_end = button->x_start + button->x_tiles * Game.tile_size.width;
    button->y_end = button->y_start + button->y_tiles * Game.tile_size.height;

    button->visible = false;

    button->type = UI_BUTTON;
    button->layer = layer;

    button->on_click = onClick;

    return button->id;
}

/**
 * All of these are now supporting/triggering drag drop
 */
int CreateItemButton(int parentId, int x, int y, UiSprite sprite, int flags = 0)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];

    button->id = id;
    button->parent_id = parentId;
    button->initialized = true;
    button->flags = flags;

    button->x_tiles = sprite.x_tiles;
    button->y_tiles = sprite.y_tiles;
    button->sprite_index = sprite.sheet_start_index;
    button->hover_sprite_index = sprite.hover_start_index;

    button->x_start = x;
    button->y_start = y;
    button->x_end = button->x_start + button->x_tiles * Game.tile_size.width;
    button->y_end = button->y_start + button->y_tiles * Game.tile_size.height;

    button->visible = false;

    button->type = UI_DRAG_DROP;
    button->layer = 2;

    button->on_click = Action_StartDrag;

    return button->id;
}

// TODO: only for uniform buttons right now
// FIXME: doesn't restrict button count correctly
IntArray CreatePanelButtons(int parentId,
                            float panelPadding,
                            float spacing,
                            int items,
                            UiSprite itemSprite,
                            int itemFlags = 0)
{
    UiElement panel = uiElements.elements[parentId];
    assert(panel.initialized);

    UniformGrid grid = CalculateGridPositions(panel,
                                              panelPadding,
                                              spacing,
                                              items,
                                              itemSprite);

    int* panelButtonIds = new int[items];

    for (int i = 0; i < grid.items; i++)
    {
        Position pos = grid.item_draw_positions[i];
        panelButtonIds[i] = CreateItemButton(panel.id,
                                             pos.x,
                                             pos.y,
                                             itemSprite,
                                             itemFlags);
    }

    return IntArray{panelButtonIds, grid.items};
}

int CreatePanel(Anchor anchor, UiSprite sprite)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];

    button->id = id;
    button->initialized = true;

    button->x_tiles = sprite.x_tiles;
    button->y_tiles = sprite.y_tiles;
    button->sprite_index = sprite.sheet_start_index;
    button->hover_sprite_index = sprite.hover_start_index;

    Position start = CalculateStartPixelPosition(anchor, sprite);

    button->x_start = start.x;
    button->y_start = start.y;
    button->x_end = button->x_start + button->x_tiles * Game.tile_size.width;
    button->y_end = button->y_start + button->y_tiles * Game.tile_size.height;

    button->visible = false;

    button->type = UI_PANEL;
    button->layer = 1;

    button->on_click = [] {};

    return button->id;
}

void SetupNullElement()
{
    // first element, is the null element
    // - how do i make it visible (obvious) that it is the null element?
    // -> Make it be in the center of the map or something?
    int id = uiElements.count++;
    UiElement* nullElement = &uiElements.elements[id];
    nullElement->visible = false;
    nullElement->x_start = Scale.render_dim.width / 2;
    nullElement->x_end = nullElement->x_start;
    nullElement->y_start = Scale.render_dim.height / 2;
    nullElement->y_end = nullElement->y_start;
    nullElement->on_click = [] {
        Log("Error: On click of null element was executed");
    };
}

void CreateMenuElements()
{
    // TODO: do i want a setup that takes previous button into account as
    // well?
    //  - so that they move automatically?
    //  - that they detect other buttons that are there, and take that
    //  position as offset? -> that would be nice, nice to define

    UiSprite startButton = UiSprite{2, 1, 12, 14, &Res.bitmaps.ui};
    UiSprite exitButton = UiSprite{2, 1, 20, 22, &Res.bitmaps.ui};
    UiSprite panel3x3 = UiSprite{3, 3, 16, 16, &Res.bitmaps.ui};
    UiSprite button2x2 = UiSprite{2,
                                  2,
                                  6 * 8 + 3,
                                  6 * 8 + 3 + 2,
                                  &Res.bitmaps.ui};

    ui.menu.map_selection_panel = CreateButton(Anchor{UPPER_MIDDLE, 0, 6},
                                               button2x2,
                                               Action_LoadUserSelectedMap);
    ui.menu.start_game_button = CreateButton(Anchor{UPPER_MIDDLE, 0, 4},
                                             startButton,
                                             Action_StartGame);
    ui.menu.exit_game_button = CreateButton(Anchor{UPPER_MIDDLE, 0, 9},
                                            exitButton,
                                            Action_Exit);
}

void CreateCraftingItems()
{
    assert(ui.crafting.tower_slots.size == 2);
    ui.crafting.tower_slot_bullet = ui.crafting.tower_slots.data[0];
    ui.crafting.tower_slot_pillar = ui.crafting.tower_slots.data[1];

    int itemCount = Res.items.count;
    assert(ui.crafting.item_slots.size >= itemCount);

    ui.crafting.item_count = itemCount;
    ui.crafting.slot_map = new EntitySlotMap[ui.crafting.item_count];

    for (int i = 0; i < itemCount; i++)
    {
        ItemData item = Res.items.items[i];
        int slotId = ui.crafting.item_slots.data[i];
        Position centerPos = ItemSlotCenter(slotId);
        int entityId = CreateItemEntity(centerPos.x, centerPos.y, item);
        ui.crafting.slot_map[i] = EntitySlotMap{slotId, entityId, slotId};
    }
}

// i might not want to show them all on startup?
// because only want to show the ones that are actually created / available
void ShowTowerButtons()
{
    int itemCount = ui.placement.item_slots.size;

    ui.placement.item_count = itemCount;
    ui.placement.slot_map = new EntitySlotMap[ui.placement.item_count];

    for (int i = 0; i < itemCount; i++)
    {
        int itemId = ui.placement.item_slots.data[i];
        UiElement* el = &uiElements.elements[itemId];

        el->visible = false;
        el->on_click = Action_SelectTowerType;
        ui.placement.slot_map[i] = EntitySlotMap{el->id, EntityZero.id, el->id};

        // first button is special and does other stuff
        if (i == 0)
        {
            el->sprite_index = 31;
            el->hover_sprite_index = 39;
            el->on_click = [] {};
            ui.placement.slot_map[i] = EntitySlotMap{NullElement.id,
                                                     EntityZero.id,
                                                     NullElement.id};
        }
    }
}

void CreateGameElements()
{
    UiSprite defaultButton = UiSprite{2, 1, 8, 10, &Res.bitmaps.ui};
    UiSprite grayButton = UiSprite{1, 1, 5, 6, &Res.bitmaps.ui};
    UiSprite craftButton = UiSprite{2, 1, 40, 42, &Res.bitmaps.ui};
    UiSprite confirmButton = UiSprite{2, 1, 44, 46, &Res.bitmaps.ui};
    UiSprite panel3x3 = UiSprite{3, 3, 16, 16, &Res.bitmaps.ui};
    UiSprite panel3x10 = UiSprite{3, 8, 16, 16, &Res.bitmaps.ui};
    UiSprite panel15x10 = UiSprite{8, 8, 16, 16, &Res.bitmaps.ui};
    UiSprite panel15x2 = UiSprite{15, 2, 16, 16, &Res.bitmaps.ui};
    UiSprite itemSlot = UiSprite{1, 1, 19, 0, &Res.bitmaps.ui};
    UiSprite bigItem = UiSprite{2, 2, 27, 29, &Res.bitmaps.ui};

    ui.crafting.tower_panel = CreatePanel(Anchor{UPPER_RIGHT, 0.5, 2},
                                          panel3x10);
    ui.crafting.items_panel = CreatePanel(Anchor{UPPER_MIDDLE, 2, 2},
                                          panel15x10);
    ui.placement.tower_selection_panel = CreatePanel(
                                            Anchor{UPPER_LEFT, -0.25, 0},
                                            panel15x2);

    ui.crafting.item_slots = CreatePanelButtons(ui.crafting.items_panel,
                                                0.5,
                                                0.25,
                                                20,
                                                itemSlot,
                                                DRAG_SOURCE);
    ui.crafting.tower_slots = CreatePanelButtons(ui.crafting.tower_panel,
                                                 0.5,
                                                 0.25,
                                                 2,
                                                 bigItem,
                                                 (DRAG_SOURCE | DROP_TARGET));
    ui.placement.item_slots = CreatePanelButtons(
                                            ui.placement.tower_selection_panel,
                                            0.5,
                                            0.25,
                                            // Do a hard limit for now, maybe
                                            // even as ristriction After that
                                            // you can't creat new ones anymore
                                            // -> so it becomes more about
                                            // strategy ?!
                                            9,
                                            grayButton,
                                            0);

    ui.crafting.show_hide_button = CreateButton(Anchor{UPPER_RIGHT, 0.5, 0.5},
                                                craftButton,
                                                Action_ToggleCraftingPanel);
    ui.crafting.crafting_button = CreateButton(Anchor{UPPER_RIGHT, 1, 8.5},
                                               confirmButton,
                                               Action_CraftTower,
                                               2);

    //-----------
    //  TODO: TMP
    ui.tmp_2 = CreateButton(Anchor{UPPER_LEFT, 0.5, 12},
                            defaultButton,
                            Action_SpawnEnemy);

    CreateCraftingItems();
    // TODO: DEBUG - should be done dynamically
    ShowTowerButtons();
}

/**
 * Initializes the storage, allocates the memory and initializes it to 0
 *
 * Creates the UI elements that are currently used
 */
void InitializeUi(int uiElementCount, int layers)
{
    uiElements.size = uiElementCount;
    uiElements.layer_count = layers;
    uiElements.elements = new UiElement[uiElementCount];

    memset(uiElements.elements, 0, sizeof(UiElement) * uiElements.size);
    SetupNullElement();

    CreateMenuElements();
    CreateGameElements();
}

// TODO: in a bigger project i should not just recreate everything
void RecreateUiElements()
{
    delete uiElements.elements;
    uiElements.count = 0;

    InitializeUi(uiElements.size, uiElements.layer_count);
}
