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

bool NotOverCenterYet(Entity entity, prox2 pos, Tile targetTile, float offset)
{
    int tileWidth = Game.tile_map.tile_size.width;
    int tileHeight = Game.tile_map.tile_size.height;
    // TODO: use util function?!
    int transformY = Game.tile_map.rows - targetTile.pos.y - 1;
    int targetPosX = targetTile.pos.x * tileWidth + tileWidth * offset - 1;
    int targetPosY = transformY * tileHeight + tileHeight * offset - 1;

    // continue moving till center of tile
    switch (entity.direction)
    {
        case NORTH:
            if (pos.y < targetPosY) return true;
            break;
        case SOUTH:
            if (pos.y > targetPosY) return true;
            break;
        case EAST:
            if (pos.x < targetPosX) return true;
            break;
        case WEST:
            if (pos.x > targetPosX) return true;
            break;
    }
    return false;
}

Direction ChooseNextDirection(Entity entity, Tile enemyTile, Tile target)
{
    // entity is at (around) center of the tile, choose next direction
    u8 neighbours = enemyTile.adjacent;
    Direction targetXDir = enemyTile.pos.x < target.pos.x ? EAST : WEST;
    Direction targetYDir = enemyTile.pos.y < target.pos.y ? NORTH : EAST;

    // there is only one case where a new direction is chosen,
    // when the he goes over the center?
    // but this can be mutliple frames -> not if i use the exact position

    Direction backDir = oppositeDirection(entity.direction);
    u8 notBackDir = ~backDir;

    u8 possibleDirections = neighbours & (EAST | WEST | NORTH | SOUTH);

    // enemy already on right of those, so no need to go further
    // just needs to go the other direction now
    bool goTowardX = enemyTile.pos.x != target.pos.x;
    bool goTowardY = enemyTile.pos.y != target.pos.y;
    if (goTowardX)
    {
        if (possibleDirections & targetXDir & notBackDir) return targetXDir;
    }
    if (goTowardY)
    {
        if (possibleDirections & targetYDir & notBackDir) return targetYDir;
    }

    // continue moving in current direction if possible
    if (possibleDirections & entity.direction) return entity.direction;

    // pick one without turning
    if (possibleDirections & (NORTH & notBackDir)) return NORTH;
    if (possibleDirections & (EAST & notBackDir)) return EAST;
    if (possibleDirections & (SOUTH & notBackDir)) return SOUTH;
    if (possibleDirections & (WEST & notBackDir)) return WEST;

    // backDir must be the only direction left
    return backDir;
}

Direction GetNextMovementDirection(Entity entity, Enemy* enemy)
{
    //!! margin, when is it over the tile? tile center? -> yea, entity is center
    Tile enemyTile = *Game.tile_map.tileAt(entity.x, entity.y);
    // if over the center, than choose the next tile
    // but only into the direction i'm moving

    // FIXME: the problem is, that we calculate the next position multiple times
    // per tile
    //  - but i want it to be only once per tile !!!!
    //  - but how to do that, how to know that?
    //  - via the over center thing? NO!
    //  - that only works for the first half!

    Tile target = *enemy->target;

    // check cross center line
    // if (NotOverTileCenterYet(entity, enemyTile, 0.5)) return
    // entity.direction;
    return ChooseNextDirection(entity, enemyTile, target);
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
        tile.tracker->entity_count = 0;
    }

    for (int i = 0; i < entities.unit_count; i++)
    {
        Entity* e = &entities.units[i];

        if (e->type == ENEMY)
        {
            Enemy* enemy = &enemies.units[e->storage_id];
            Tile enemyTile = *Game.tile_map.tileAt(e->x, e->y);
            Tile target = *enemy->target;

            if (enemyTile.tile_id != PATH_TILE) continue;
            // FIXME: BUG - this is why i don't quite go towards the end
            //  - becaues i stop here prematurely, becaues not going till center
            if (enemyTile.pos.x == target.pos.x &&
                enemyTile.pos.y == target.pos.y)
            {
                // still continue moving till over the center line
                if (!NotOverCenterYet(*e,
                                      prox2{e->move_x, e->move_y},
                                      target,
                                      1))
                {
                    if (enemy->state != TARGET_LOCATION)
                    {
                        enemy->state = TARGET_LOCATION;
                        Logf("Enemy reached destination: %d %d", e->x, e->y);
                    }
                    continue;
                }
            }

            float increase = Time.sim_time * enemy->speed;
            switch (e->direction)
            {
                case NORTH: e->move_y += increase; break;
                case EAST: e->move_x += increase; break;
                case SOUTH: e->move_y -= increase; break;
                case WEST: e->move_x -= increase; break;
            }

            // checking if at center switch point
            // before current move we were not over the center
            // but after current move we are
            // means now we need to determine a new position
            if (!NotOverCenterYet(*e,
                                  prox2{e->move_x, e->move_y},
                                  enemyTile,
                                  0.5) &&
                NotOverCenterYet(*e,
                                 prox2{(float)e->x, (float)e->y},
                                 enemyTile,
                                 0.5))
            {
                // FIXME: this never evaluates to true for some reason ...
                Direction moveDir = GetNextMovementDirection(*e, enemy);
                Logf("Updating entity %d direction to %d", e->id, moveDir >> 4);

                e->direction = moveDir;
            }

            // if move y & move-x are over center, and x+y are not
            // -> chose new position!

            // casting to integer position
            e->y = e->move_y;
            e->x = e->move_x;

            Tile* newEnemyTile = Game.tile_map.tileAt(e->x, e->y);
            newEnemyTile->tracker->entity_ids[newEnemyTile->tracker->entity_count++] =
                                                    e->id;
        }
    }
}

struct Vector2
{
    float x, y;
};

Vector2 normalize(int originX, int originY, int destX, int destY)
{
    int xDir = destX - originX;
    int yDir = destY - originY;

    if (xDir == 0 && yDir == 0) return Vector2{0, 0};
    if (xDir == 0)
    {
        float dir = yDir > 0 ? 1 : -1;
        return Vector2{0, dir};
    }
    if (yDir == 0)
    {
        float dir = xDir > 0 ? 1 : -1;
        return Vector2{dir, 0};
    }

    float xRatio = (float)xDir / yDir;
    float magnitude = sqrt(pow(xDir, 2) + pow(yDir, 2));

    float normalizedX = xDir / magnitude;
    float normalizedY = yDir / magnitude;

    return Vector2{normalizedX, normalizedY};
}

void MoveProjectiles()
{
    for (int i = 0; i < projectiles.unit_count; i++)
    {
        Projectile* p = &projectiles.units[i];

        if (p->state != ACTIVE) continue;

        Entity* e = &entities.units[p->entity_id];

        // time in seconds * speed per tile + pixels per tile
        float increase = Time.sim_time * p->speed * Game.tile_size.height;
        Vector2 normalized = normalize(e->x, e->y, p->target_x, p->target_y);

        if (normalized.x == 0 && normalized.y == 0)
        {
            p->state = DESTROYED;
        }

        e->move_x += increase * normalized.x;
        e->move_y += increase * normalized.y;

        e->x = e->move_x;
        e->y = e->move_y;
    }
}
