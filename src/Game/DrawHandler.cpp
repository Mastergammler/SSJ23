#include "internal.h"

// TODO: the index is kind of hardcoded and not flexible
//  Change this for some kind of sheet/index mapping
void DrawTilemap(Tilemap& map, ScreenBuffer& buffer, SpriteSheet& sheet)
{
    int* tile = map.idMap;
    for (int y = 0; y < map.rows; y++)
    {
        for (int x = 0; x < map.columns; x++)
        {
            int tileId = *tile++;
            int tileIdx = y * map.columns + x;
            int tileX = x * TileSize.width;
            int tileY = y * TileSize.height;

            int sheetIdx;

            if (tileId == 0) { sheetIdx = 9; }
            else if (tileId == 1)
            {
                ContextTile tile = map.context_tiles[tileIdx];
                TileState ts;

                // is special case - for inner corners
                if (tile.adjacent > 0b11110000)
                {
                    ts = (TileState)tile.adjacent;
                }
                else
                {
                    // only compare the first 4 bits
                    ts = (TileState)(tile.adjacent & 0b11110000);
                }

                sheetIdx = PathMap[ts];
            }

            DrawBitmap(buffer, sheet.tiles[sheetIdx], tileX, tileY);
        }
    }
}
