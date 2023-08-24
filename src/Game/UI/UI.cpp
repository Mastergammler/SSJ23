#include "../ui.h"
#include "../../Win32/module.h"
#include "../internal.h"
#include "../systems.h"

MouseState mouseState;
UiElementStorage uiElements;
Navigation navigation;

Position CalculateStartPixelPosition(UiPosition position,
                                     float borderOffset,
                                     int xTiles,
                                     int yTiles)
{

    int pixelOffsetX =
        borderOffset > 0 ? borderOffset * (_tileSize.width) - 1 : 0;
    int pixelOffsetY =
        borderOffset > 0 ? borderOffset * (_tileSize.height) - 1 : 0;

    switch (position)
    {
        case UPPER_LEFT:
        {
            int x = 0;
            int y = _tileMap.rows * _tileSize.height - 1;
            y -= yTiles * _tileSize.height;
            x += pixelOffsetX;
            y -= pixelOffsetY;
            return Position{x, y};
        }
        case UPPER_RIGHT:
        {
            int x = _tileMap.columns * _tileSize.width - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= xTiles * _tileSize.width;
            y -= yTiles * _tileSize.height;
            x -= pixelOffsetX;
            y -= pixelOffsetY;
            return Position{x, y};
        }
        case UPPER_MIDDLE:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= xTiles * _tileSize.width / 2;
            y -= yTiles * _tileSize.height;
            y -= pixelOffsetY;
            return Position{x, y};
        }
        case CENTERED:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height / 2 - 1;
            x -= xTiles * _tileSize.width / 2;
            y -= yTiles * _tileSize.height / 2;
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
                 bool visible)
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
                                                 button->y_tiles);

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

int CreatePanel(UiPosition pos,
                int xSize,
                int ySize,
                float offset,
                bool visible)
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
                                                 button->y_tiles);

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

    int middleTile = _tileMap.columns / 2;

    // how would i want to define this?
    // more like Left corner, right corner middle etc
    // and than a offset to each border
    // - so how to do that?
    //
    // to uniformely lay out i need to know about the other ui parts ...

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

    ui.crafting_panel_id = CreatePanel(UPPER_LEFT, 8, 4, 0.5, false);
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
