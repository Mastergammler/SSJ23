#include "../internal.h"

ComponentStore components;
EntityStore entities;
TowerStore towers;
EnemyStore enemies;
ItemStore items;
CannonTypeStore cannons;
ProjectileStore projectiles;

void InitializeProjectileStore(int storeCount, int poolSize)
{
    assert(poolSize <= storeCount);
    // TODO: does this make sense to have a different pool size than general
    // allocated memory?
    projectiles.size = storeCount;
    projectiles.units = new Projectile[storeCount];
    projectiles.pool_index = 0;
    projectiles.pool_size = poolSize;
    memset(projectiles.units, 0, sizeof(Projectile) * storeCount);
}

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
    e->x = Scale.render_dim.width / 2;
    e->y = Scale.render_dim.height / 2;
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
    InitializeProjectileStore(storeCount, 512);
}

Entity* InitNextEntity()
{
    if (entities.unit_count % 256 == 0)
    {
        Logf("Entity count reached: %d - Fps Sample: %d",
             entities.unit_count,
             Time.fps);
    }
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->initialized = true;

    return e;
}

// TODO: handle pooling if necessary
int CreateNewProjectile(int entityId)
{
    assert(projectiles.unit_count < projectiles.size);

    int id = projectiles.unit_count++;
    Projectile* p = &projectiles.units[id];

    p->entity_id = entityId;
    p->storage_id = id;
    p->state = DESTROYED;

    return p->storage_id;
}

int CreateProjectileEntity()
{
    if (projectiles.pool_size <= projectiles.unit_count)
    {
        // use pooling - if nothing found after 10, create new
        for (int i = 0; i < 10; i++)
        {
            Projectile p = projectiles.units[projectiles.pool_index++];
            if (projectiles.pool_index >= projectiles.pool_size)
            {
                projectiles.pool_index = 0;
            }

            if (p.state == DESTROYED)
            {
                Collider* coll = &components.memory[p.entity_id].collider;
                coll->x_offset = 0;
                coll->y_offset = 0;
                coll->height = 8;
                coll->width = 8;

                return p.entity_id;
            }
        }
    }
    Entity* e = InitNextEntity();
    e->storage_id = CreateNewProjectile(e->id);

    e->type = PROJECTILE;
    e->component_mask = (COLLIDER);

    Collider* coll = &components.memory[e->id].collider;
    coll->x_offset = 0;
    coll->y_offset = 0;
    coll->height = 8;
    coll->width = 8;

    return e->id;
}

int CreateEnemy(int entityId, int speed)
{
    assert(enemies.unit_count < enemies.size);

    int id = enemies.unit_count++;
    Enemy* e = &enemies.units[id];

    e->storage_id = id;
    e->entity_id = entityId;

    e->speed = speed;
    e->state = WALKING;

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

    e->component_mask = (ANIMATOR | COLLIDER);
    Animator* anim = &components.memory[e->id].animator;
    anim->initialized = true;
    anim->looping = true;
    anim->sample_index = 0;
    anim->sample_count = animation.sprite_count;
    anim->time_per_sample = animation.time_per_sprite;
    anim->samples = animation.sprites;

    Collider* coll = &components.memory[e->id].collider;
    coll->x_offset = 0;
    coll->y_offset = 0;
    coll->height = 12;
    coll->width = 12;

    return e->id;
}

/**
 * Modified version of the fibonnaci sequence
 * x = 4 + SIG[(x-1) * 8]
 */
int calculateTileCountFoRange(int range)
{
    if (range <= 0) return 0;

    int acc = 4;

    for (int r = 1; r <= range; r++)
    {
        acc += (r - 1) * 8;
    }

    return acc;
}

struct TileArray
{
    Tile** tiles;
    int count;
};

TileArray determineRelevantTiles(int x, int y, int radius)
{
    Tile* towerTile = Game.tile_map.tileAt(x, y);

    int maxNumTiles = calculateTileCountFoRange(radius);
    // for adding center tile, simpler to iterate through
    Tile** tmp = new Tile*[maxNumTiles + 1];

    int pathTileCount = 0;

    // -1 because we ignore the first cross one
    int tilesPerRow = (radius - 1) * 2 + 1;
    int rows = (radius - 1) * 2 + 1;

    // TODO: include boundaries
    Tile* ulStart = towerTile - (radius - 1) * Game.tile_map.columns -
                    (radius - 1);

    for (int r = 0; r < rows; r++)
    {
        Tile* currentRowTile = ulStart + r * Game.tile_map.columns;
        for (int x = 0; x < tilesPerRow; x++)
        {
            Tile* cpy = currentRowTile++;
            if (cpy->tile_id == PATH_TILE)
            {
                // don't forget to copy, because we move the other pointer
                tmp[pathTileCount++] = cpy;
            }
        }
    }

    // Add extra cross
    Tile* crossLeft = towerTile - radius;
    if (crossLeft->tile_id == PATH_TILE)
    {
        tmp[pathTileCount++] = crossLeft;
    }

    Tile* crossRight = towerTile + radius;
    if (crossRight->tile_id == PATH_TILE)
    {
        tmp[pathTileCount++] = crossRight;
    }

    Tile* crossTop = towerTile - radius * Game.tile_map.columns;
    if (crossTop->tile_id == PATH_TILE)
    {
        tmp[pathTileCount++] = crossTop;
    }

    Tile* crossBot = towerTile + radius * Game.tile_map.columns;
    if (crossBot->tile_id == PATH_TILE)
    {
        tmp[pathTileCount++] = crossBot;
    }

    Tile** relevantTilesActual = new Tile*[pathTileCount];
    copy(tmp, tmp + pathTileCount, relevantTilesActual);
    delete[] tmp;

    return TileArray{relevantTilesActual, pathTileCount};
}

// TODO: based on value
int CreateTower(int entityId,
                int x,
                int y,
                int cannonTypeId,
                Sprite bullet,
                Sprite pillar)
{
    assert(towers.unit_count < towers.size);

    int id = towers.unit_count++;
    Tower* t = &towers.units[id];
    CannonType cannon = cannons.units[cannonTypeId];

    t->storage_id = id;
    t->entity_id = entityId;

    t->radius = cannon.range;
    t->fire_rate = cannon.fire_rate;
    t->bullet_speed = cannon.bullet_speed;

    t->initialized = true;
    t->pillar_sprite = pillar;
    t->bullet_sprite = bullet;

    TileArray relevantTiles = determineRelevantTiles(x, y, t->radius);
    t->relevant_tiles = relevantTiles.tiles;
    t->tile_count = relevantTiles.count;

    return t->storage_id;
}

int CreateTowerEntity(int x,
                      int y,
                      int cannonTypeId,
                      Sprite bulletSprite,
                      Sprite pillarSprite)
{
    Entity* e = InitNextEntity();
    e->storage_id = CreateTower(e->id,
                                x,
                                y,
                                cannonTypeId,
                                bulletSprite,
                                pillarSprite);

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

    Item bullet = items.units[bulletId];
    Item pillar = items.units[postId];

    float avPower = (bullet.power + pillar.power) / 2;
    t->range = 2 * bullet.aero + avPower - bullet.weight;
    t->fire_rate = 2 * pillar.stability * pillar.sturdieness / 3 + avPower / 3 +
                   0.8;
    t->bullet_speed = 3 * avPower * 1 / bullet.weight + 1;

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
