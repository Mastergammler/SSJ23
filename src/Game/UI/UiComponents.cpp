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
