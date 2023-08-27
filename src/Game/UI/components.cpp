#include "../internal.h"

Position ItemSlotCenter(int slotId)
{
    UiElement* el = &uiElements.elements[slotId];

    int centerX = el->x_start + _tileSize.width * el->x_tiles / 2 - 1;
    int centerY = el->y_start + _tileSize.height * el->y_tiles / 2 - 1;

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
            x += xOffsetPixel;
            return Position{x, y};
        }
        case CENTERED:
        {
            int x = _tileMap.columns * _tileSize.width / 2 - 1;
            int y = _tileMap.rows * _tileSize.height / 2 - 1;
            x -= sprite.x_tiles * _tileSize.width / 2;
            y -= sprite.y_tiles * _tileSize.height / 2;
            y -= yOffsetPixel;
            x += xOffsetPixel;

            return Position{x, y};
        }
    };
}

UniformGrid CalculateGridPositions(UiElement panel,
                                   float padding,
                                   float spacing,
                                   int items,
                                   Sprite sprite)
{
    // these are width -> not indices
    int pixelPadding = padding * _tileSize.width;
    int minPixelSpacing = spacing * _tileSize.width;

    int buttonSizeX = _tileSize.width * sprite.x_tiles;
    int buttonSizeY = _tileSize.height * sprite.y_tiles;
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
             panel.id,
             maxButtonCount);
    }

    int spaceButtonOnlyX = buttonsPerRow * _tileSize.width * sprite.x_tiles;
    int spaceButtonOnlyY = buttonsPerColumn * _tileSize.height * sprite.y_tiles;
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
    int areaStartY = panel.y_end - pixelPadding -
                     _tileSize.height * sprite.y_tiles;

    areaStartX += leftoverSpaceX / 2;
    areaStartY -= leftoverSpaceY / 2;

    // surface = offset
    int buttonSurfaceX = buttonSizeX + actualSpacingX;
    int buttonSurfaceY = buttonSizeY + actualSpacingY;

    int rowsToDraw = items / buttonsPerRow;
    int lastRowItems = items % buttonsPerRow;
    if (lastRowItems != 0)
        rowsToDraw++;
    else
        lastRowItems = buttonsPerRow;

    Position* itemPositions = new Position[items];
    int itemPosIndex = 0;

    for (int y = 0; y < rowsToDraw - 1; y++)
    {
        for (int x = 0; x < buttonsPerRow; x++)
        {
            int xPos = areaStartX + x * buttonSurfaceX;
            int yPos = areaStartY - y * buttonSurfaceY;

            itemPositions[itemPosIndex++] = Position{xPos, yPos};
        }
    }

    // draw last row - because they might not have all items
    for (int x = 0; x < lastRowItems; x++)
    {
        int xPos = areaStartX + x * buttonSurfaceX;
        int yPos = areaStartY - (rowsToDraw - 1) * buttonSurfaceY;

        itemPositions[itemPosIndex++] = Position{xPos, yPos};
    }

    return UniformGrid{rowsToDraw, buttonsPerRow, items, itemPositions};
}
