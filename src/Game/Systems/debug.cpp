#include "../internal.h"

float lastPrintTime = 0;
const float PRINT_THRESHOLD_MS = 500. / 1000;

void Debug_PrintEnemyTilePositions()
{
    lastPrintTime += measure.frame_delta_time;
    if (lastPrintTime < PRINT_THRESHOLD_MS) return;
    lastPrintTime -= PRINT_THRESHOLD_MS;

    for (int i = 0; i < Game.tile_map.tile_count; i++)
    {
        Tile tile = Game.tile_map.tiles[i];

        if (tile.entities->entity_count > 0)
        {
            Logf("Tile %d (%d,%d) contains the following enemies",
                 i,
                 tile.x,
                 tile.y);

            for (int e = 0; e < tile.entities->entity_count; e++)
            {
                Logf("  id: %d", tile.entities->entity_ids[e]);
            }
        }
    }
}

const u32 DEBUG_COLOR = 0xFFE8FA;

void Debug_DrawEntityMovementPossibilities(ScreenBuffer buffer)
{
    for (int i = 0; i < entities.unit_count; ++i)
    {
        Entity e = entities.units[i];

        if (e.type != ENEMY) continue;

        Tile enemyTile = *Game.tile_map.tileAt(e.x, e.y);

        if (enemyTile.adjacent & NORTH)
        {
            DrawLine(buffer, e.x, e.y, e.x, e.y + 12, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & SOUTH)
        {
            DrawLine(buffer, e.x, e.y - 12, e.x, e.y, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & EAST)
        {
            DrawLine(buffer, e.x, e.y, e.x + 12, e.y, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & WEST)
        {
            DrawLine(buffer, e.x - 12, e.y, e.x, e.y, DEBUG_COLOR);
        }
    }
}

Sprite aim = Sprite{1, 1, 57, &Res.bitmaps.ui};
Sprite target = Sprite{1, 1, 58, &Res.bitmaps.ui};

void Debug_DrawTowerRangeAndDetection(ScreenBuffer buffer)
{
    // TODO: first hav to set the tiles on the
    for (int i = 0; i < towers.unit_count; ++i)
    {
        Tower tower = towers.units[i];

        for (int t = 0; t < tower.tile_count; t++)
        {
            Tile tile = *tower.relevant_tiles[t];

            int drawX = tile.x * Game.tile_size.width;
            // +1 because idx 0 would not be the max position to draw
            int drawY = MirrorY((tile.y + 1) * Game.tile_size.height,
                                scale.draw_height);

            // TODO: draw if enemy found
            DrawTiles(buffer,
                      drawX,
                      drawY,
                      *aim.sheet,
                      aim.sheet_start_index,
                      aim.x_tiles,
                      aim.y_tiles);
        }
    }
}
