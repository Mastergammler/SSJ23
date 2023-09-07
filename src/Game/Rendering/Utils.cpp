#include "../internal.h"

Position TileToDrawPosition(Tile tile)
{
    int x = tile.pos.x * Game.tile_size.width;
    int y = MirrorY((tile.pos.y + 1) * Game.tile_size.height,
                    Scale.render_dim.height);

    return Position{x, y};
}

Position TileCenterPosition(Tile tile)
{
    Position tilePixelCenter = TileToDrawPosition(tile);

    tilePixelCenter.x += Game.tile_size.width / 2 - 1;
    tilePixelCenter.y += Game.tile_size.height / 2 - 1;

    return tilePixelCenter;
}
