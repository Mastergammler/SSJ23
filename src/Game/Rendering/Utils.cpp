#include "../internal.h"

function v2 TileToDrawPosition(Tile tile)
{
    int x = tile.pos.x * Game.tile_size.width;
    int y = MirrorY((tile.pos.y + 1) * Game.tile_size.height,
                    Scale.render_dim.height);

    return v2{x, y};
}

function v2 TileCenterPosition(Tile tile)
{
    v2 tilePixelCenter = TileToDrawPosition(tile);
    v2 offset = {Game.tile_size.width / 2 - 1, Game.tile_size.height / 2 - 1};

    return tilePixelCenter + offset;
}
