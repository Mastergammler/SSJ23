#include "../internal.h"

EntityStore entities;
TowerStore towers;
EnemyStore enemies;
ComponentStore components;

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

void InitializeComponentStorage(int storeCount)
{
    components.size = storeCount;
    components.memory = new EntityComponents[storeCount];
    memset(components.memory, 0, sizeof(EntityComponents) * storeCount);
}

void InitializeEntities(int storeCount)
{
    entities.size = storeCount;
    entities.units = new Entity[storeCount];
    memset(entities.units, 0, sizeof(Entity) * storeCount);

    InitializeComponentStorage(storeCount);
    InitializeTowerStorage(storeCount);
    InitializeEnemyStorage(storeCount);
}

Entity* InitNextEntity()
{
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->initialized = true;

    return e;
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

// TODO: how to put the animation data in there?
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation)
{
    Entity* e = InitNextEntity();
    e->storage_id = CreateEnemy(e->id, speed);

    e->x = x;
    e->y = y;
    e->move_x = x;
    e->move_y = y;
    e->type = ENEMY;
    e->direction = direction;
    e->sprite = sprite;

    e->component_mask = (ANIMATOR);
    Animator* anim = &components.memory[e->id].animator;
    anim->initialized = true;
    anim->looping = true;
    anim->sample_index = 0;
    anim->sample_count = animation.sprite_count;
    anim->time_per_sample = animation.time_per_sprite;
    anim->samples = animation.sprites;

    return e->id;
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
    Entity* e = InitNextEntity();
    e->storage_id = CreateTower(e->id);

    e->x = x;
    e->y = y;
    // tower doesn't need it
    // e->move_x = x;
    // e->move_y = y;
    e->type = TOWER;
    e->direction = NORTH;
    e->sprite = sprite;

    return e->id;
}

int CreateItemEntity(int x, int y, Sprite sprite)
{
    Entity* e = InitNextEntity();
    // TODO: no storage yet

    e->x = x;
    e->y = y;

    e->type = CRAFT_ITEM;
    e->sprite = sprite;

    return e->id;
}
