#include "../internal.h"

Position ItemSlotCenter(int slotId)
{
    UiElement* el = &uiElements.elements[slotId];

    int centerX = el->x_start + _tileSize.width / 2;
    int centerY = el->y_start + _tileSize.height / 2;

    return Position{centerX, centerY};
}

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
Position CalculateStartPixelPosition(Anchor anchor, Sprite sprite)
{
    // no -1 needed, because an offset, is a length!
    int xOffsetPixel = anchor.x_offset * _tileSize.width;
    int yOffsetPixel = anchor.y_offset * _tileSize.height;

    switch (anchor.reference_point)
    {
        case UPPER_LEFT:
        {
            int x = 0;
            int y = _tileMap.rows * _tileSize.height - 1;
            y -= sprite.y_tiles * _tileSize.height;
            x += xOffsetPixel;
            y -= yOffsetPixel;
            return Position{x, y};
        }
        case UPPER_RIGHT:
        {
            int x = _tileMap.columns * _tileSize.width - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= sprite.x_tiles * _tileSize.width;
            y -= sprite.y_tiles * _tileSize.height;
            x -= xOffsetPixel;
            y -= yOffsetPixel;
            return Position{x, y};
        }
        case UPPER_MIDDLE:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height - 1;
            x -= sprite.x_tiles * _tileSize.width / 2;
            y -= sprite.y_tiles * _tileSize.height;
            y -= yOffsetPixel;
            return Position{x, y};
        }
        case CENTERED:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height / 2 - 1;
            x -= sprite.x_tiles * _tileSize.width / 2;
            y -= sprite.y_tiles * _tileSize.height / 2;
            return Position{x, y};
        }
    };
}
