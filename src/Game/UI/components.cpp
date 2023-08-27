#include "../internal.h"

Position ItemSlotCenter(int slotId)
{
    UiElement* el = &uiElements.elements[slotId];

    int centerX = el->x_start + Game.tile_size.width * el->x_tiles / 2 - 1;
    int centerY = el->y_start + Game.tile_size.height * el->y_tiles / 2 - 1;

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
    int xOffsetPixel = anchor.x_offset * Game.tile_size.width;
    int yOffsetPixel = anchor.y_offset * Game.tile_size.height;

    switch (anchor.reference_point)
    {
        case UPPER_LEFT:
        {
            int x = 0;
            int y = Game.tile_map.rows * Game.tile_size.height - 1;
            y -= sprite.y_tiles * Game.tile_size.height;
            x += xOffsetPixel;
            y -= yOffsetPixel;
            return Position{x, y};
        }
        case UPPER_RIGHT:
        {
            int x = Game.tile_map.columns * Game.tile_size.width - 1;
            int y = Game.tile_map.rows * Game.tile_size.height - 1;
            x -= sprite.x_tiles * Game.tile_size.width;
            y -= sprite.y_tiles * Game.tile_size.height;
            x -= xOffsetPixel;
            y -= yOffsetPixel;
            return Position{x, y};
        }
        case UPPER_MIDDLE:
        {
            int x = Game.tile_map.columns * Game.tile_size.width / 2 - 1;
            int y = Game.tile_map.rows * Game.tile_size.height - 1;
            x -= sprite.x_tiles * Game.tile_size.width / 2;
            y -= sprite.y_tiles * Game.tile_size.height;
            y -= yOffsetPixel;
            x += xOffsetPixel;
            return Position{x, y};
        }
        case CENTERED:
        {
            int x = Game.tile_map.columns * Game.tile_size.width / 2 - 1;
            int y = Game.tile_map.rows * Game.tile_size.height / 2 - 1;
            x -= sprite.x_tiles * Game.tile_size.width / 2;
            y -= sprite.y_tiles * Game.tile_size.height / 2;
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
    int pixelPadding = padding * Game.tile_size.width;
    int minPixelSpacing = spacing * Game.tile_size.width;

    int buttonSizeX = Game.tile_size.width * sprite.x_tiles;
    int buttonSizeY = Game.tile_size.height * sprite.y_tiles;
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

    int spaceButtonOnlyX = buttonsPerRow * Game.tile_size.width *
                           sprite.x_tiles;
    int spaceButtonOnlyY = buttonsPerColumn * Game.tile_size.height *
                           sprite.y_tiles;
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
                     Game.tile_size.height * sprite.y_tiles;

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
