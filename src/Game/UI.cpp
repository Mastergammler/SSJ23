#include "internal.h"

UiElementStorage storage;
UiElement NullElement = {.visible = false,
                         .initialized = false,
                         .id = -1,
                         .layer = -1,
                         .on_click = [] {}};

/**
 * Initializes the storage, allocates the memory and initializes it to 0
 */
void InitializeUi(int uiElementCount, int layers)
{
    storage.size = uiElementCount;
    storage.layer_count = layers;
    storage.elements = new UiElement[uiElementCount];
    memset(storage.elements, 0, sizeof(UiElement) * storage.size);
}

// TODO: these are hardcoded initializers now
//  dunno what would be a better way, apart from just recreating the whole
//  struct or putting hundreds of thing in there maybe some layout default? ->
//  but i think this is good enough for now

int CreateButton(int mapX, int mapY, float offset, Action onClick)
{
    assert(storage.count < storage.size);

    int id = storage.count++;
    UiElement* button = &storage.elements[id];
    int offsetPixel = tileSize.width * offset;

    button->id = id;
    button->initialized = true;

    button->x_tiles = 2;
    button->y_tiles = 1;
    button->x_start = tileSize.width * (tileMap.columns - mapX) + offsetPixel;
    button->y_start = tileSize.height * (tileMap.rows - mapY) + offsetPixel;
    button->x_end = button->x_start + button->x_tiles * tileSize.width;
    button->y_end = button->y_start + button->y_tiles * tileSize.height;
    button->visible = true;

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
    assert(storage.count < storage.size);

    int id = storage.count++;
    UiElement* button = &storage.elements[id];
    int offsetPixel = tileSize.width * offset;

    button->id = id;
    button->initialized = true;

    button->x_tiles = xSize;
    button->y_tiles = ySize;
    button->x_start = tileSize.width * tileX + offsetPixel;
    button->y_start = tileSize.height * tileY + offsetPixel;
    button->x_end = button->x_start + button->x_tiles * tileSize.width;
    button->y_end = button->y_start + button->y_tiles * tileSize.height;
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
    actionState.ui_focus = hovered->id != -1;

    if (mouseState.left_clicked) { hovered->on_click(); }
    if (mouseState.right_clicked) { Action_ToggleTowerPreview(); }
}

//-------------
//  SYSTEMS
//-------------

void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    for (int l = 0; l < storage.layer_count; ++l)
    {
        for (int i = 0; i < storage.count; ++i)
        {
            UiElement* cur = &storage.elements[i];
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
    for (int i = 0; i < storage.count; ++i)
    {
        UiElement* cur = &storage.elements[i];
        if (cur->visible)
        {
            if (cur->x_start <= x && cur->x_end >= x && cur->y_start <= y &&
                cur->y_end >= y)
            {
                if (cur->layer > foundElement->layer) { foundElement = cur; }
            }
        }
    }

    return foundElement;
}
