#include "internal.h"

UiElementStorage storage;
UiElement NullElement = {.visible = false,
                         .initialized = false,
                         .id = -1,
                         .layer = -1,
                         .on_click = [] {}};

void InitializeUi(int uiElementCount)
{
    storage.size = uiElementCount;
    storage.elements = new UiElement[uiElementCount];
    memset(storage.elements, 0, sizeof(UiElement) * storage.size);
}

int CreateButton(Tilemap& map, int mapX, int mapY, float offset, Action onClick)
{
    assert(storage.count < storage.size);

    int id = storage.count++;
    UiElement* button = &storage.elements[id];
    int offsetPixel = tileSize.width * offset;

    button->id = id;
    button->x_start = tileSize.width * (map.columns - mapX) + offsetPixel;
    button->y_start = tileSize.height * (map.rows - mapY) + offsetPixel;
    button->x_end = button->x_start + 2 * tileSize.width;
    button->y_end = button->y_start + tileSize.height;

    // TODO: hardcoded values
    button->sprite_index = 8;
    button->hover_sprite_index = 10;
    button->layer = 0;
    button->visible = true;
    button->initialized = true;
    button->type = UI_SINGLE;
    button->x_tiles = 2;
    button->y_tiles = 1;

    button->on_click = onClick;

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

void RenderButtons(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    for (int i = 0; i < storage.count; ++i)
    {
        UiElement* cur = &storage.elements[i];
        if (cur->visible)
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
            // TODO: kind of a workaround
            cur->hovered = false;
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
