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

int CreateEnemy(int entityId)
{
    assert(enemies.unit_count < enemies.size);

    int id = enemies.unit_count++;
    Enemy* e = &enemies.units[id];

    e->storage_id = id;
    e->entity_id = entityId;

    // TODO:
    e->speed = 20;

    e->initialized = true;

    return e->storage_id;
}

int CreateEnemyEntity(int x, int y, Sprite sprite, Direction direction)
{
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->storage_id = CreateEnemy(e->id);

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

const float MOVEMENT_MOD = 0.1;

void MoveEnemies()
{
    // 8 ms/frame
    // move speed of 20
    // how many pixels per second?
    // -> about 2? =>
    // => Constant is around / 10

    // maybe rather calculate on which time signature i have to move him?
    // -> hmm that's bad also, then all would always only move synchronized
    // => That's wired (feature for spooky stuff)

    for (int i = 0; i < entities.unit_count; i++)
    {
        Entity* e = &entities.units[i];

        if (e->type == ENEMY)
        {
            Enemy enemy = enemies.units[e->storage_id];

            float increase = measure.delta_time * MOVEMENT_MOD * enemy.speed;

            if (e->direction == WEST)
            {
                e->move_x += increase;
                e->x = e->move_x;
            }
            else if (e->direction == SOUTH)
            {
                // TODO: do i move up or down again? uff
                e->move_y -= increase;
                e->y = e->move_y;
            }
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

        int drawStartX = e.x - (tileSize.width / 2 - 1);
        int drawStartY = e.y - (tileSize.height / 2 - 1);

        DrawTiles(buffer,
                  drawStartX,
                  drawStartY,
                  *e.sprite.sheet,
                  e.sprite.sheet_start_index,
                  e.sprite.x_tiles,
                  e.sprite.y_tiles);
    }
}
