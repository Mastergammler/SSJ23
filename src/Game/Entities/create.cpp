#include "../internal.h"

ComponentStore components;
EntityStore entities;
TowerStore towers;
EnemyStore enemies;
ItemStore items;
CannonTypeStore cannons;

void InitializeItemStorage(int storeCount)
{
    cannons.size = storeCount;
    cannons.units = new CannonType[storeCount];
    memset(cannons.units, 0, sizeof(CannonType) * storeCount);
}

void InitializeCannonTypeStorage(int storeCount)
{
    items.size = storeCount;
    items.units = new Item[storeCount];
    memset(items.units, 0, sizeof(Item) * storeCount);
}

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

void InitEntityZero()
{
    Entity* e = &entities.units[entities.unit_count++];
    e->x = scale.draw_width / 2;
    e->y = scale.draw_height / 2;
}

void InitializeEntities(int storeCount)
{
    entities.size = storeCount;
    entities.units = new Entity[storeCount];
    memset(entities.units, 0, sizeof(Entity) * storeCount);
    // TODO: doesn't work with rendering -> because still gets found
    // crashes then because no sprite
    // InitEntityZero();

    InitializeComponentStorage(storeCount);
    InitializeTowerStorage(storeCount);
    InitializeEnemyStorage(storeCount);
    InitializeItemStorage(storeCount);
    InitializeCannonTypeStorage(storeCount);
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

int CreateTower(int entityId, Sprite bullet, Sprite pillar)
{
    assert(towers.unit_count < towers.size);

    int id = towers.unit_count++;
    Tower* t = &towers.units[id];

    t->storage_id = id;
    t->entity_id = entityId;

    // TODO:
    t->radius = 0;
    t->initialized = true;
    t->pillar_sprite = pillar;
    t->bullet_sprite = bullet;

    return t->storage_id;
}

int CreateTowerEntity(int x, int y, Sprite bulletSprite, Sprite pillarSprite)
{
    Entity* e = InitNextEntity();
    e->storage_id = CreateTower(e->id, bulletSprite, pillarSprite);

    e->x = x;
    e->y = y;
    // tower doesn't need it
    e->move_x = 0;
    e->move_y = 0;
    e->type = TOWER;
    e->direction = NORTH;
    e->sprite = bulletSprite;

    return e->id;
}

int CreateItem(int entityId, ItemData data)
{
    assert(items.unit_count < items.size);

    int id = items.unit_count++;
    Item* i = &items.units[id];

    i->initialized = true;
    i->storage_id = id;
    i->entity_id = entityId;

    i->sturdieness = data.sturdieness;
    i->stability = data.stability;
    i->power = data.power;
    i->aero = data.aero;
    i->weight = data.weight;
    i->effects = data.effect_types;

    i->bullet_sprite_idx = data.bullet_sprite_idx;
    i->pillar_sprite_idx = data.pillar_sprite_idx;

    // TODO: how to map these and put these in?
    i->hit_sound_idx = 0;
    i->shoot_sound_idx = 0;

    return i->storage_id;
}

int CreateItemEntity(int x, int y, ItemData data)
{
    Entity* e = InitNextEntity();
    Sprite sprite = Sprite{1, 1, data.bullet_sprite_idx, &Res.bitmaps.items};

    e->x = x;
    e->y = y;
    e->storage_id = CreateItem(e->id, data);

    e->type = CRAFT_ITEM;
    e->sprite = sprite;
    e->initialized = true;

    return e->id;
}

int CreateCannonType(int entityId, int bulletId, int postId)
{
    assert(cannons.unit_count < cannons.size);
    int id = cannons.unit_count++;
    CannonType* t = &cannons.units[id];

    t->storage_id = id;
    t->entity_id = entityId;

    t->bullet_item_id = bulletId;
    t->pillar_item_id = postId;
    t->initialized = true;

    return t->storage_id;
}

/**
 */
int CreatCannonTypeEntity(int x, int y, int bulletItemId, int postItemId)
{
    Entity* e = InitNextEntity();

    Item bullet = items.units[bulletItemId];
    // take bullet type as sprite
    Sprite sprite = Sprite{1, 1, bullet.bullet_sprite_idx, &Res.bitmaps.items};

    e->x = x;
    e->y = y;
    e->storage_id = CreateCannonType(e->id, bulletItemId, postItemId);

    e->type = TOWER_PROTO;
    e->sprite = sprite;
    e->initialized = true;

    return e->id;
}
