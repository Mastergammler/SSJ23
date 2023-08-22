#include "internal.h"

EntityStore entities;
TowerStore towers;

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
    e->type = TOWER;
    e->direction = NORTH;
    e->sprite = sprite;

    e->initialized = true;

    return id;
};

//-------------
//  SYSTEMS
//-------------

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
