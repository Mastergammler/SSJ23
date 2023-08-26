#include "../internal.h"
#include "../ui.h"

UiElementStorage uiElements;

// TODO: still has a few hard coded initializers
int CreateButton(Anchor anchor, UiSprite sprite, Action onClick)
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
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

    button->visible = false;

    button->type = UI_BUTTON;
    button->layer = 0;

    button->on_click = onClick;

    return button->id;
}

/**
 * All of these are now supporting/triggering drag drop
 */
int CreateItemButton(int parentId, int x, int y, UiSprite sprite)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];

    button->id = id;
    button->parent_id = parentId;
    button->initialized = true;

    button->x_tiles = sprite.x_tiles;
    button->y_tiles = sprite.y_tiles;
    button->sprite_index = sprite.sheet_start_index;
    button->hover_sprite_index = sprite.hover_start_index;

    button->x_start = x;
    button->y_start = y;
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

    button->visible = false;

    button->type = UI_DRAG_DROP;
    button->layer = 2;

    button->on_click = Action_StartDrag;

    return button->id;
}

// TODO: only for uniform buttons right now
// TEST: bigger sized buttons
IntArray CreatePanelButtons(int parentId,
                            float panelPadding,
                            float spacing,
                            int items,
                            UiSprite itemSprite)
{

    UiElement panel = uiElements.elements[parentId];
    assert(panel.initialized);

    // these are width -> not indices
    int pixelPadding = panelPadding * _tileSize.width;
    int minPixelSpacing = spacing * _tileSize.width;

    int buttonSizeX = _tileSize.width * itemSprite.x_tiles;
    int buttonSizeY = _tileSize.height * itemSprite.y_tiles;
    int buttonSpaceX = buttonSizeX + minPixelSpacing;
    int buttonSpaceY = buttonSizeY + minPixelSpacing;

    int areaSurfacePixelX = panel.x_end - panel.x_start - 2 * pixelPadding;
    int areaSurfacePixelY = panel.y_end - panel.y_start - 2 * pixelPadding;

    // 1 spacing is removed, because it's done by padding
    int buttonsPerRow = (areaSurfacePixelX + minPixelSpacing) / buttonSpaceX;
    int buttonsPerColumn = (areaSurfacePixelY + minPixelSpacing) / buttonSpaceY;
    int maxButtonCount = buttonsPerRow * buttonsPerColumn;

    if (maxButtonCount < items)
    {
        Logf("Panel %d can only fit %d items, additional items are "
             "ignored!",
             parentId,
             maxButtonCount);
    }

    int spaceButtonOnlyX = buttonsPerRow * _tileSize.width;
    int spaceButtonOnlyY = buttonsPerColumn * _tileSize.height;
    int availableSpacingSpaceX = areaSurfacePixelX - spaceButtonOnlyX;
    int availableSpacingSpaceY = areaSurfacePixelY - spaceButtonOnlyY;

    // only spaces in between buttons
    int actualSpacingX = buttonsPerRow > 1 ? availableSpacingSpaceX / (buttonsPerRow -
                                                                       1)
                                           : 0;
    int actualSpacingY = buttonsPerColumn > 1 ? availableSpacingSpaceY / (buttonsPerColumn -
                                                                          1)
                                              : 0;
    int leftoverSpaceX = buttonsPerRow > 1 ? availableSpacingSpaceX % (buttonsPerRow -
                                                                       1)
                                           : 0;
    int leftoverSpaceY = buttonsPerColumn > 1 ? availableSpacingSpaceY % (buttonsPerColumn -
                                                                          1)
                                              : 0;

    int areaStartX = panel.x_start + pixelPadding;
    int areaStartY = panel.y_end - pixelPadding - _tileSize.height;

    areaStartX += leftoverSpaceX / 2;
    areaStartY -= leftoverSpaceY / 2;

    // surface = offset
    int buttonSurfaceX = buttonSizeX + actualSpacingX;
    int buttonSurfaceY = buttonSizeY + actualSpacingY;

    int rowsToDraw = items / buttonsPerRow;
    int lastRowItems = items % buttonsPerRow;
    if (lastRowItems != 0) rowsToDraw++;

    // return it properly
    int* panelButtonIds = new int[items];
    int idAccIdx = 0;

    for (int y = 0; y < rowsToDraw - 1; y++)
    {
        for (int x = 0; x < buttonsPerRow; x++)
        {
            int xPos = areaStartX + x * buttonSurfaceX;
            int yPos = areaStartY - y * buttonSurfaceY;

            panelButtonIds[idAccIdx++] = CreateItemButton(parentId,
                                                          xPos,
                                                          yPos,
                                                          itemSprite);
        }
    }

    // draw last row - because they might not have all items
    for (int x = 0; x < lastRowItems; x++)
    {
        int xPos = areaStartX + x * buttonSurfaceX;
        int yPos = areaStartY - (rowsToDraw - 1) * buttonSurfaceY;

        panelButtonIds[idAccIdx++] = CreateItemButton(parentId,
                                                      xPos,
                                                      yPos,
                                                      itemSprite);
    }

    return IntArray{panelButtonIds, items};
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
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

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
    nullElement->x_start = scale.draw_width / 2;
    nullElement->x_end = nullElement->x_start;
    nullElement->y_start = scale.draw_height / 2;
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

    ui.menu.map_selection_panel = CreatePanel(Anchor{UPPER_MIDDLE, 0, 6},
                                              panel3x3);
    ui.menu.start_game_button = CreateButton(Anchor{UPPER_MIDDLE, 0, 4},
                                             startButton,
                                             Action_StartGame);
    ui.menu.exit_game_button = CreateButton(Anchor{UPPER_MIDDLE, 0, 10},
                                            exitButton,
                                            Action_Exit);
}

void CreateGameElements()
{
    UiSprite defaultButton = UiSprite{2, 1, 8, 10, &Res.bitmaps.ui};
    UiSprite panel3x3 = UiSprite{3, 3, 16, 16, &Res.bitmaps.ui};
    UiSprite panel2x10 = UiSprite{2, 10, 16, 16, &Res.bitmaps.ui};
    UiSprite panel15x10 = UiSprite{15, 10, 16, 16, &Res.bitmaps.ui};
    UiSprite panel15x2 = UiSprite{15, 2, 16, 16, &Res.bitmaps.ui};
    UiSprite itemSlot = UiSprite{1, 1, 19, 0, &Res.bitmaps.ui};

    ui.crafting.tower_panel = CreatePanel(Anchor{UPPER_RIGHT, 0.5, 3},
                                          panel2x10);
    ui.crafting.items_panel = CreatePanel(Anchor{UPPER_MIDDLE, 0, 3},
                                          panel15x10);
    ui.placement.tower_selection_panel = CreatePanel(Anchor{UPPER_LEFT, 2, 2.5},
                                                     panel15x2);
    ui.crafting.item_slots = CreatePanelButtons(ui.crafting.items_panel,
                                                0.5,
                                                0.25,
                                                20,
                                                itemSlot);
    ui.crafting.tower_slots = CreatePanelButtons(ui.crafting.tower_panel,
                                                 0.5,
                                                 0.25,
                                                 6,
                                                 itemSlot);

    ui.crafting.show_hide_button = CreateButton(Anchor{UPPER_RIGHT, 0.5, 0.5},
                                                defaultButton,
                                                Action_ToggleCraftingPanel);

    //-----------
    //  TODO: TMP
    ui.tmp_2 = CreateButton(Anchor{UPPER_MIDDLE, 0.5, 0.5},
                            defaultButton,
                            Action_SpawnEnemy);

    Sprite cat = Sprite{1, 1, 1, &Res.bitmaps.items};
    int itemCount = 18;
    assert(ui.crafting.item_slots.size >= itemCount);

    for (int i = 0; i < itemCount; i++)
    {
        int slotId = ui.crafting.item_slots.data[i];
        Position centerPos = ItemSlotCenter(slotId);
        int entityId = CreateItemEntity(centerPos.x, centerPos.y, cat);
        ui.crafting.item_slot_mapping[slotId] = EntitySlotMap{slotId,
                                                              entityId,
                                                              slotId};
    }
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
