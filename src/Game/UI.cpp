#include "internal.h"

UiElementStorage storage;

void InitializeUi(int uiElementCount)
{
    storage.size = uiElementCount;
    storage.elements = new UiElement[uiElementCount];
    memset(storage.elements, 0, sizeof(UiElement) * storage.size);
    Log("Initialized Storage");
}

int CreateButton(Tilemap& map, int mapX, int mapY, float offset)
{
    assert(storage.count < storage.size);

    int id = storage.count++;
    UiElement* button = &storage.elements[id];
    int offsetPixel = TileSize.width * offset;

    button->id = id;
    button->x_start = TileSize.width * (map.columns - mapX) + offsetPixel;
    button->y_start = TileSize.height * (map.rows - mapY) + offsetPixel;
    button->x_end = button->x_start + 2 * TileSize.width;
    button->y_end = button->y_start + TileSize.height;

    button->sprite_index = 8;
    button->layer = 0;
    button->visible = true;
    button->initialized = true;
    button->type = UI_SINGLE;
    button->x_tiles = 2;
    button->y_tiles = 1;

    return button->id;
}

// this Would be the system?
void RenderButtons(ScreenBuffer& buffer, SpriteSheet& sheet)
{

    for (int i = 0; i < storage.count; ++i)
    {
        UiElement cur = storage.elements[i];
        if (cur.visible)
        {
            DrawTiles(buffer,
                      cur.x_start,
                      cur.y_start,
                      sheet,
                      cur.sprite_index,
                      cur.x_tiles,
                      cur.y_tiles);
        }
    }
}
