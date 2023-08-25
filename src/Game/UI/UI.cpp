#include "../ui.h"
#include "../../Win32/module.h"
#include "../internal.h"
#include "../systems.h"

MouseState mouseState;
UiElementStorage uiElements;
Navigation navigation;

/**
 * Border offset is the no of tiles offset from the border position
 * (For center it has no impact)
 * yOffset is the offset in tiles from the top
 * it is an additional offset and stacks with the border offset
 *
 * What is offset is dependant on the UiPosition in use
 * On Top + offset moves downward
 * (Things use top down perspective)
 */
Position CalculateStartPixelPosition(UiPosition position,
                                     float borderOffset,
                                     int xTiles,
                                     int yTiles,
                                     float yOffset)
{

    int pixelBorderOffsetX =
        borderOffset > 0 ? borderOffset * _tileSize.width - 1 : 0;
    int pixelBorderOffsetY =
        borderOffset > 0 ? borderOffset * _tileSize.height - 1 : 0;
    int pixelYOffset = yOffset > 0 ? yOffset * _tileSize.height - 1 : 0;

    switch (position)
    {
        case UPPER_LEFT:
        {
            int x = 0;
            int y = _tileMap.rows * _tileSize.height - 1;
            y -= yTiles * _tileSize.height;
            x += pixelBorderOffsetX;
            y -= pixelBorderOffsetY;
            y -= pixelYOffset;
            return Position{x, y};
        }
        case UPPER_RIGHT:
        {
            int x = _tileMap.columns * _tileSize.width - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= xTiles * _tileSize.width;
            y -= yTiles * _tileSize.height;
            x -= pixelBorderOffsetX;
            y -= pixelBorderOffsetY;
            y -= pixelYOffset;
            return Position{x, y};
        }
        case UPPER_MIDDLE:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= xTiles * _tileSize.width / 2;
            y -= yTiles * _tileSize.height;
            y -= pixelBorderOffsetY;
            y -= pixelYOffset;
            return Position{x, y};
        }
        case CENTERED:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height / 2 - 1;
            x -= xTiles * _tileSize.width / 2;
            y -= yTiles * _tileSize.height / 2;
            y -= pixelYOffset;
            return Position{x, y};
        }
    };
}

// TODO: these are hardcoded initializers now
//  dunno what would be a better way, apart from just recreating the whole
//  struct or putting hundreds of thing in there maybe some layout default?
//  -> but i think this is good enough for now
//  - Working with relative positions (also not to mess it up on map change)
//  - todo rework for sprite input?
int CreateButton(UiPosition pos,
                 float offset,
                 int spriteIndex,
                 int hoverIndex,
                 Action onClick,
                 bool visible,
                 float yOffset = 0)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];
    int offsetPixel = _tileSize.width * offset;

    button->id = id;
    button->initialized = true;

    button->x_tiles = 2;
    button->y_tiles = 1;

    Position start = CalculateStartPixelPosition(pos,
                                                 offset,
                                                 button->x_tiles,
                                                 button->y_tiles,
                                                 yOffset);

    button->x_start = start.x;
    button->y_start = start.y;
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

    button->visible = visible;

    button->type = UI_SINGLE;
    button->sprite_index = spriteIndex;
    button->hover_sprite_index = hoverIndex;
    button->layer = 0;

    button->on_click = onClick;

    return button->id;
}

int CreateItemButton(int parentId, int x, int y, bool visible)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];

    button->id = id;
    button->parent_id = parentId;
    button->initialized = true;

    button->x_tiles = 1;
    button->y_tiles = 1;

    button->x_start = x;
    button->y_start = y;
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

    button->visible = visible;

    button->type = UI_SINGLE;
    button->sprite_index = 19;
    button->hover_sprite_index = 0;
    button->layer = 2;

    button->on_click = [] {};

    return button->id;
}

// for now only uniform 1x1 buttons
IntArray CreatePanelButtons(int parentId,
                            float panelPadding,
                            float spacing,
                            int items)
{

    UiElement panel = uiElements.elements[parentId];
    assert(panel.initialized);

    // these are width -> not indices
    int pixelPadding = panelPadding * _tileSize.width;
    int minPixelSpacing = spacing * _tileSize.width;

    int buttonSizeX = _tileSize.width * 1;
    int buttonSizeY = _tileSize.height * 1;
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
        Logf("Panel %d can only fit %d items, additional items are ignored!",
             parentId,
             maxButtonCount);
    }

    int spaceButtonOnlyX = buttonsPerRow * _tileSize.width;
    int spaceButtonOnlyY = buttonsPerColumn * _tileSize.height;
    int availableSpacingSpaceX = areaSurfacePixelX - spaceButtonOnlyX;
    int availableSpacingSpaceY = areaSurfacePixelY - spaceButtonOnlyY;

    // only spaces in between buttons
    int actualSpacingX =
        buttonsPerRow > 1 ? availableSpacingSpaceX / (buttonsPerRow - 1) : 0;
    int actualSpacingY = buttonsPerColumn > 1
                             ? availableSpacingSpaceY / (buttonsPerColumn - 1)
                             : 0;
    int leftoverSpaceX =
        buttonsPerRow > 1 ? availableSpacingSpaceX % (buttonsPerRow - 1) : 0;
    int leftoverSpaceY = buttonsPerColumn > 1
                             ? availableSpacingSpaceY % (buttonsPerColumn - 1)
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

            panelButtonIds[idAccIdx++] =
                CreateItemButton(parentId, xPos, yPos, false);
        }
    }

    // draw last row - because they might not have all items
    for (int x = 0; x < lastRowItems; x++)
    {
        int xPos = areaStartX + x * buttonSurfaceX;
        int yPos = areaStartY - (rowsToDraw - 1) * buttonSurfaceY;

        panelButtonIds[idAccIdx++] =
            CreateItemButton(parentId, xPos, yPos, false);
    }

    return IntArray{panelButtonIds, items};
}

int CreatePanel(UiPosition pos,
                int xSize,
                int ySize,
                float offset,
                bool visible,
                float yOffset = 0)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];
    int offsetPixel = _tileSize.width * offset;

    button->id = id;
    button->initialized = true;

    button->x_tiles = xSize;
    button->y_tiles = ySize;
    Position start = CalculateStartPixelPosition(pos,
                                                 offset,
                                                 button->x_tiles,
                                                 button->y_tiles,
                                                 yOffset);

    button->x_start = start.x;
    button->y_start = start.y;
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;

    button->visible = visible;

    button->type = UI_PANEL;
    button->sprite_index = 16;
    button->hover_sprite_index = button->sprite_index;
    button->layer = 1;

    button->on_click = [] {};

    return button->id;
}

void ProcessMouseActions()
{
    UiElement* hovered = FindHighestLayerCollision(mouseState.x, mouseState.y);
    // TODO: kinda ugly, not a good solution
    hovered->hovered = true;
    ui.ui_focus = hovered->id != -1;

    if (mouseState.left_clicked)
    {
        hovered->on_click();
        Action_PlaceTower();
    }
    if (mouseState.right_clicked)
    {
        Action_ToggleTowerPreview();
    }
}

void UpdateMouseState()
{
    mouseState.left_clicked = false;
    mouseState.left_released = false;
    mouseState.right_clicked = false;
    mouseState.right_released = false;

    mouseState.x = mouse.x;
    mouseState.y = mouse.y;

    bool leftClickedNew = mouse.buttons & MOUSE_LEFT;
    bool rightClickedNew = mouse.buttons & MOUSE_RIGHT;

    if (leftClickedNew != mouseState.left_down)
    {
        if (leftClickedNew)
            mouseState.left_clicked = true;
        else
            mouseState.left_released = true;
    }

    if (rightClickedNew != mouseState.right_down)
    {
        if (rightClickedNew)
            mouseState.right_clicked = true;
        else
            mouseState.right_released = true;
    }

    mouseState.left_down = leftClickedNew;
    mouseState.right_down = rightClickedNew;
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

    // TODO: do i want a setup that takes previous button into account as well?
    //  - so that they move automatically?
    //  - that they detect other buttons that are there, and take that position
    //  as offset? -> that would be nice, nice to define

    ui.map_selection_panel_id = CreatePanel(UPPER_MIDDLE, 3, 3, 6, true);
    ui.start_game_button_id = CreateButton(
        UPPER_MIDDLE,
        4,
        12,
        14,
        [] {
            PlayAudioFile(&_audio.click_lo, false, 90);
            Action_StartGame();
        },
        true);
    ui.exit_game_button_id = CreateButton(
        UPPER_MIDDLE,
        10,
        20,
        22,
        [] { running = false; },
        true);

    // Game UI
    ui.tower_crafting_panel_id =
        CreatePanel(UPPER_RIGHT, 2, 10, 0.5, false, 2.5);
    ui.items_panel_id = CreatePanel(UPPER_MIDDLE, 15, 10, 0, false, 3);
    ui.tower_selection_panel_id = CreatePanel(UPPER_LEFT, 15, 2, 0.5, false);
    ui.item_slots = CreatePanelButtons(ui.items_panel_id, 0.5, 0.25, 20);
    CreatePanelButtons(ui.tower_crafting_panel_id, 0.5, 0.25, 6);

    ui.tmp_1 = CreateButton(
        UPPER_RIGHT,
        0.5,
        8,
        10,
        [] {
            PlayAudioFile(&_audio.click_hi, false, 90);
            Action_ToggleCraftingPanel();
        },
        false);
    ui.tmp_2 = CreateButton(
        UPPER_MIDDLE,
        0.5,
        8,
        10,
        [] {
            PlayAudioFile(&_audio.pop_hi, false, 90);
            Action_SpawnEnemy();
        },
        false);
}
