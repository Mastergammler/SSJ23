#include "internal.h"

EntityStore entities;
TowerStore towers;
EnemyStore enemies;

void InitializeEnemyStorage(int storeCount)
{
    enemies.size = storeCount;
    enemies.units = new Enemy[storeCount];
    memset(enemies.units, 0, sizeof(Enemy) * storeCount);
}

void InitializeTowerStorage(int storeCount)
{
    towers.size = storeCount;
    towers.units = new Tower[storeCount];
    memset(towers.units, 0, sizeof(Tower) * storeCount);
}

void InitializeEntities(int storeCount)
{
    entities.size = storeCount;
    entities.units = new Entity[storeCount];
    memset(entities.units, 0, sizeof(Entity) * storeCount);

    InitializeTowerStorage(storeCount);
    InitializeEnemyStorage(storeCount);
}

int CreateEnemy(int entityId, int speed)
{
    assert(enemies.unit_count < enemies.size);

    int id = enemies.unit_count++;
    Enemy* e = &enemies.units[id];

    e->storage_id = id;
    e->entity_id = entityId;

    e->speed = speed;

    e->initialized = true;

    return e->storage_id;
}

int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed)
{
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->storage_id = CreateEnemy(e->id, speed);

    e->x = x;
    e->y = y;
    e->move_x = x;
    e->move_y = y;
    e->type = ENEMY;
    e->direction = direction;
    e->sprite = sprite;

    e->initialized = true;

    return id;
}

int CreateTower(int entityId)
{
    assert(towers.unit_count < towers.size);

    int id = towers.unit_count++;
    Tower* t = &towers.units[id];

    t->storage_id = id;
    t->entity_id = entityId;

    // TODO:
    t->radius = 0;

    t->initialized = true;

    return t->storage_id;
}

int CreateTowerEntity(int x, int y, Sprite sprite)
{
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->storage_id = CreateTower(e->id);

    e->x = x;
    e->y = y;
    // tower doesn't need it
    // e->move_x = x;
    // e->move_y = y;
    e->type = TOWER;
    e->direction = NORTH;
    e->sprite = sprite;

    e->initialized = true;

    return id;
};

//-------------
//  SYSTEMS
//-------------

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

Direction GetNextMovementDirection(Entity entity)
{
    //!! margin, when is it over the tile? tile center? -> yea, entity is center
    Tile enemyTile = *_tileMap.tileAt(entity.x, entity.y);
    // TODO: multiple targets?
    Tile target = *_tileMap.targets[0];

    int targetPositionX =
        enemyTile.x * _tileMap.tile_size.width + _tileMap.tile_size.width / 2;
    int targetPositionY =
        enemyTile.y * _tileMap.tile_size.height + _tileMap.tile_size.height / 2;

    // move towards the center
    if (entity.x < targetPositionX || entity.y < targetPositionY)
        return entity.direction;

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

    for (int i = 0; i < entities.unit_count; i++)
    {
        Entity* e = &entities.units[i];

        if (e->type == ENEMY)
        {
            Enemy enemy = enemies.units[e->storage_id];
            Tile enemyTile = *_tileMap.tileAt(e->x, e->y);
            // TODO: multiple targets case?
            Tile target = *_tileMap.targets[0];

            if (enemyTile.tile_id != PATH_TILE) continue;
            if (enemyTile.x == target.x && enemyTile.y == target.y) continue;

            Direction moveDir = GetNextMovementDirection(*e);
            float increase = measure.frame_delta_time * SPEED_MOD * enemy.speed;
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

        Tile enemyTile = *_tileMap.tileAt(e.x, e.y);

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

void RenderEntities(ScreenBuffer buffer)
{

    // determine render order / TODO: is this performant enough?
    vector<int> renderOrder(entities.unit_count);
    for (int i = 0; i < entities.unit_count; ++i)
    {
        renderOrder[i] = i;
    }

    sort(renderOrder.begin(), renderOrder.end(), [&](int a, int b) {
        return entities.units[a].y > entities.units[b].y;
    });

    for (int i = 0; i < entities.unit_count; ++i)
    {
        int renderOrderIndex = renderOrder[i];
        Entity e = entities.units[renderOrderIndex];

        int drawStartX = e.x - (_tileSize.width / 2 - 1);
        int drawStartY = e.y - (_tileSize.height / 2 - 1);

        DrawTiles(buffer,
                  drawStartX,
                  drawStartY,
                  *e.sprite.sheet,
                  e.sprite.sheet_start_index,
                  e.sprite.x_tiles,
                  e.sprite.y_tiles);
    }
}
