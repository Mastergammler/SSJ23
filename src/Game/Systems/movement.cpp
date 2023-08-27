#include "../internal.h"

// TODO: with better encocding i could have just bitshifted?
//  N S E W - then just (dir << 2) or (dir >> 2)
//  but still would not know in which direction?
Direction oppositeDirection(Direction dir)
{
    switch (dir)
    {
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        case EAST: return WEST;
        case WEST: return EAST;
    }
};

bool ReachedOverCenterBound(Entity entity, Tile targetTile, float offset)
{
    int tileWidth = Game.tile_map.tile_size.width;
    int tileHeight = Game.tile_map.tile_size.height;
    int transformY = Game.tile_map.rows - targetTile.y - 1;
    int targetPosX = targetTile.x * tileWidth + tileWidth * offset - 1;
    int targetPosY = transformY * tileHeight + tileHeight * offset - 1;

    // continue moving till center of tile
    switch (entity.direction)
    {
        case NORTH:
            if (entity.y < targetPosY) return true;
            break;
        case SOUTH:
            if (entity.y > targetPosY) return true;
            break;
        case EAST:
            if (entity.x < targetPosX) return true;
            break;
        case WEST:
            if (entity.x > targetPosX) return true;
            break;
    }
    return false;
}

Direction GetNextMovementDirection(Entity entity)
{
    //!! margin, when is it over the tile? tile center? -> yea, entity is center
    Tile enemyTile = *Game.tile_map.tileAt(entity.x, entity.y);
    // TODO: multiple targets?
    Tile target = *Game.tile_map.targets[0];

    // check cross center line
    if (ReachedOverCenterBound(entity, enemyTile, 0.5)) return entity.direction;

    // entity is at (around) center of the tile, choose next direction
    u8 neighbours = enemyTile.adjacent;
    Direction targetXDir = enemyTile.x < target.x ? EAST : WEST;
    Direction targetYDir = enemyTile.y < target.y ? NORTH : EAST;

    Direction backDir = oppositeDirection(entity.direction);
    u8 notBackDir = ~backDir;

    u8 possibleDirections = neighbours & (EAST | WEST | NORTH | SOUTH);
    if (possibleDirections & entity.direction) return entity.direction;
    if (possibleDirections & targetXDir & notBackDir) return targetXDir;
    if (possibleDirections & targetYDir & notBackDir) return targetYDir;

    // pick one without turning
    if (possibleDirections & (NORTH & notBackDir)) return NORTH;
    if (possibleDirections & (EAST & notBackDir)) return EAST;
    if (possibleDirections & (SOUTH & notBackDir)) return SOUTH;
    if (possibleDirections & (WEST & notBackDir)) return WEST;

    // backDir must be the only direction left
    return backDir;
}

const float SPEED_MOD = 0.1;

void MoveEnemies()
{
    // maybe rather calculate on which time signature i have to move him?
    // -> hmm that's bad also, then all would always only move synchronized
    // => That's wired (feature for spooky stuff)

    // clean tilemap position caches
    for (int i = 0; i < Game.tile_map.tile_count; ++i)
    {
        Tile tile = Game.tile_map.tiles[i];
        tile.entities->entity_count = 0;
    }

    for (int i = 0; i < entities.unit_count; i++)
    {
        Entity* e = &entities.units[i];

        if (e->type == ENEMY)
        {
            Enemy enemy = enemies.units[e->storage_id];
            Tile enemyTile = *Game.tile_map.tileAt(e->x, e->y);

            // TODO: multiple targets case?
            Tile target = *Game.tile_map.targets[0];

            if (enemyTile.tile_id != PATH_TILE) continue;
            if (enemyTile.x == target.x && enemyTile.y == target.y)
            {
                if (ReachedOverCenterBound(*e, target, 1)) continue;
            }

            Direction moveDir = GetNextMovementDirection(*e);
            float increase = measure.frame_delta_time * enemy.speed;
            switch (moveDir)
            {
                case NORTH: e->move_y += increase; break;
                case EAST: e->move_x += increase; break;
                case SOUTH: e->move_y -= increase; break;
                case WEST: e->move_x -= increase; break;
            }

            // casting to integer position
            e->y = e->move_y;
            e->x = e->move_x;
            e->direction = moveDir;

            Tile* newEnemyTile = Game.tile_map.tileAt(e->x, e->y);
            newEnemyTile->entities->entity_ids[newEnemyTile->entities->entity_count++] =
                                                    e->id;
        }
    }
}
