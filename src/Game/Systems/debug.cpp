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
