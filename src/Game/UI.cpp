#include "internal.h"

UiElementStorage uiElements;
UiElement NullElement = {.visible = false,
                         .initialized = false,
                         .id = -1,
                         .layer = -1,
                         .on_click = [] {}};

int CreateButton(int mapX,
                 int mapY,
                 float offset,
                 Action onClick,
                 bool visible);
int CreatePanel(int tileX,
                int tileY,
                int xSize,
                int ySize,
                float offset,
                bool visible);

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

    ui.crafting_panel_id = CreatePanel(2, 10, 8, 4, 0.5, false);

    int btn1 = CreateButton(
        3,
        2,
        0.5,
        [] {
            PlayAudioFile(&_audio.click_hi, false, 90);
            Action_ToggleCraftingPanel();
        },
        true);
    int btn3 = CreateButton(
        10,
        2,
        0.5,
        [] {
            PlayAudioFile(&_audio.pop_hi, false, 90);
            Action_SpawnEnemy();
        },
        true);

    int btn2 = CreateButton(
        5,
        4,
        0.5,
        [] {
            PlayAudioFile(&_audio.click_lo, false, 90);
            Action_ToggleTowerPreview();
        },
        false);
    int btn4 = CreateButton(
        12,
        2,
        0.5,
        [] { PlayAudioFile(&_audio.pop_lo, false, 90); },
        false);
}

// TODO: these are hardcoded initializers now
//  dunno what would be a better way, apart from just recreating the whole
//  struct or putting hundreds of thing in there maybe some layout default? ->
//  but i think this is good enough for now
//  - Working with relative positions (also not to mess it up on map change)

int CreateButton(int mapX, int mapY, float offset, Action onClick, bool visible)
{
    assert(uiElements.count < uiElements.size);

    int id = uiElements.count++;
    UiElement* button = &uiElements.elements[id];
    int offsetPixel = _tileSize.width * offset;

    button->id = id;
    button->initialized = true;

    button->x_tiles = 2;
    button->y_tiles = 1;
    button->x_start = _tileSize.width * (_tileMap.columns - mapX) + offsetPixel;
    button->y_start = _tileSize.height * (_tileMap.rows - mapY) + offsetPixel;
    button->x_end = button->x_start + button->x_tiles * _tileSize.width;
    button->y_end = button->y_start + button->y_tiles * _tileSize.height;
    button->visible = visible;

    button->type = UI_SINGLE;
    button->sprite_index = 8;
    button->hover_sprite_index = 10;
    button->layer = 0;

    button->on_click = onClick;

    return button->id;
}

int CreatePanel(int tileX,
                int tileY,
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
    button->x_start = _tileSize.width * tileX + offsetPixel;
    button->y_start = _tileSize.height * tileY + offsetPixel;
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

//-------------
//  SYSTEMS
//-------------

void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    for (int l = 0; l < uiElements.layer_count; ++l)
    {
        for (int i = 0; i < uiElements.count; ++i)
        {
            UiElement* cur = &uiElements.elements[i];
            if (!cur->visible || cur->layer != l) continue;

            if (cur->type == UI_PANEL)
            {
                DrawPanel(buffer,
                          cur->x_start,
                          cur->y_start,
                          sheet,
                          cur->sprite_index,
                          cur->x_tiles,
                          cur->y_tiles);
            }
            else
            {
                int spriteIdx =
                    cur->hovered ? cur->hover_sprite_index : cur->sprite_index;
                DrawTiles(buffer,
                          cur->x_start,
                          cur->y_start,
                          sheet,
                          spriteIdx,
                          cur->x_tiles,
                          cur->y_tiles);
                // TODO: kind of a workaround, dunno how to do this better atm
                cur->hovered = false;
            }
        }
    }
}

UiElement* FindHighestLayerCollision(int x, int y)
{
    UiElement* foundElement = &NullElement;
    for (int i = 0; i < uiElements.count; ++i)
    {
        UiElement* cur = &uiElements.elements[i];
        if (cur->visible)
        {
            if (cur->x_start <= x && cur->x_end >= x && cur->y_start <= y &&
                cur->y_end >= y)
            {
                if (cur->layer > foundElement->layer)
                {
                    foundElement = cur;
                }
            }
        }
    }

    return foundElement;
}
