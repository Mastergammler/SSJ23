// #include "../../operators.h"
#include "../internal.h"

struct TileArray
{
    Tile** tiles;
    int count;
};

void InitEntityZero()
{
    Entity* e = &entities.units[entities.unit_count++];
    e->x = Scale.render_dim.width / 2;
    e->y = Scale.render_dim.height / 2;
}

int CycleCount = 0;
const int MIN_IN_MS = 60000;

Entity* InitNextEntity()
{
    // performance log every minute
    int checkAgaints = Time.CheckTimeSinceStart() / MIN_IN_MS;
    if (checkAgaints > CycleCount)
    {
        Logf("%d Min - Entity count reached: %d (enemies : %d | projectiles: "
             "%d) "
             "- Fps Sample: %d",
             checkAgaints,
             entities.unit_count,
             enemies.unit_count,
             projectiles.unit_count,
             Time.fps);
        CycleCount++;
    }
    assert(entities.unit_count < entities.size);

    int id = entities.unit_count++;
    Entity* e = &entities.units[id];

    e->id = id;
    e->initialized = true;

    return e;
}

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
    Entity* e = NULL;
    if (projectiles.pool_size <= projectiles.unit_count &&
        projectiles.pool_size > 0)
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

                e = &entities.units[p.entity_id];
                break;
            }
        }
    }

    if (!e)
    {
        // automatic pooling
        projectiles.pool_size++;
        assert(projectiles.pool_size <= projectiles.size);

        e = InitNextEntity();
        e->storage_id = CreateNewProjectile(e->id);
    }

    e->type = PROJECTILE;
    e->component_mask = (COLLIDER);

    Collider* coll = &components.memory[e->id].collider;
    coll->x_offset = 0;
    coll->y_offset = 0;
    coll->height = 8;
    coll->width = 8;

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

/**
 * Gets the tile of the cross based on the value, either horizontally or
 * vertically
 */
Tile* crossTileByOffset(Tile* towerTile, int value, bool horizontal)
{
    if (horizontal)
    {
        // for x offset values
        if (-value > towerTile->pos.x || value > Game.tile_map.columns - 1)
            return NULL;
    }
    else
    {
        // for y offset values
        if (-value > towerTile->pos.y || value > Game.tile_map.rows - 1)
            return NULL;
        value = value * Game.tile_map.columns;
    }

    Tile* cross = towerTile + value;
    if (cross->tile_id == PATH_TILE)
    {
        return cross;
    }
    return NULL;
};

Tile* tileAtPosition(int tileX, int tileY)
{
    return &Game.tile_map.tiles[tileX + tileY * Game.tile_map.columns];
}

TileArray determineRelevantTiles(int x, int y, int radius)
{
    Tile* towerTile = Game.tile_map.tileAt(x, y);
    int maxNumTiles = calculateTileCountFoRange(radius);

    // +1 for adding center tile, simpler to iterate through
    Tile** tmp = new Tile*[maxNumTiles + 1];

    int pathTileCount = 0;

    // -1 because for 1 we only have the cross
    // only after the cross we get a whole tile round on range
    int tileCirclesRange = (radius - 1);

    v2 towardUL = v2{-1, 1} * LL_UL;
    v2 towardLR = v2{1, -1} * LL_UL;
    v2 unclampedUL = towerTile->pos + tileCirclesRange * towardUL;
    v2 unclampedLR = towerTile->pos + tileCirclesRange * towardLR;
    v2 rangeUL = Clamp(unclampedUL,
                       Game.tile_map.min_pos,
                       Game.tile_map.max_pos);
    v2 rangeLR = Clamp(unclampedLR,
                       Game.tile_map.min_pos,
                       Game.tile_map.max_pos);

    for (int y = rangeUL.y; y <= rangeLR.y; y++)
    {
        for (int x = rangeUL.x; x <= rangeLR.x; x++)
        {
            Tile* t = tileAtPosition(x, y);
            if (t->tile_id == PATH_TILE)
            {
                tmp[pathTileCount++] = t;
            }
        }
    }

    // Add the extra cross
    // TODO: use vectors here too?!
    Tile* leftCross = crossTileByOffset(towerTile, -radius, true);
    if (leftCross) tmp[pathTileCount++] = leftCross;

    Tile* crossTile = crossTileByOffset(towerTile, radius, true);
    if (crossTile) tmp[pathTileCount++] = crossTile;

    Tile* topTile = crossTileByOffset(towerTile, -radius, false);
    if (topTile) tmp[pathTileCount++] = topTile;

    Tile* botTile = crossTileByOffset(towerTile, radius, false);
    if (botTile) tmp[pathTileCount++] = botTile;

    // TODO: do i have a 1 off here? Because the COUNT is bigger than the index?
    Tile** relevantTilesActual = new Tile*[pathTileCount];
    copy(tmp, tmp + pathTileCount, relevantTilesActual);
    delete[] tmp;

    return TileArray{relevantTilesActual, pathTileCount};
}

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
    t->bullet_effects = cannon.bullet_effects;

    t->radius = cannon.range;
    t->fire_rate = cannon.fire_rate;
    t->bullet_speed = cannon.bullet_speed;
    t->breaking_probability = cannon.breaking_probability;

    if (t->breaking_probability >= 100)
        t->state = TOWER_BROKEN;
    else
        t->state = TOWER_ACTIVE;

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
                      Sprite pillarSprite,
                      Sprite brokenSprite)
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

    // to be shown, when the tower is broken
    e->sprite = brokenSprite;

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

// NOTE: works quite good, maybe i can tweak it a bit to be even more extrem at
// the end
//  becaues often things that should almost never break, still break after ahile
//  this is because if you have breaking probabilty 2, its still 1/50 shots
//  this might still be too much, but it's ok for now
//  - also i might need to work with float values
//  - because else i don't get the desired accuracy at the end
//  -> This might be one reason for it not working so well
//  -> Because as of right now 1% is the lowest i can currently do
int CalculateBreakingProbabilty(int sturdieness,
                                int stability,
                                int weight,
                                int power)
{
    int stabiltyValue = sturdieness + stability - (weight * power) + 1;
    int stabilityNormalized = (stabiltyValue + 3) * 15;

    stabilityNormalized = stabilityNormalized < 0 ? 0 : stabilityNormalized;
    stabilityNormalized = stabilityNormalized > 100 ? 100 : stabilityNormalized;

    // 0 gives us 1, and 100 gives us 0
    // using this non linear scale, becaues a probabilty of 90 breaking after 10
    // shots feels wrong with that we have a probability of ca 5% breaking for
    // 75, 50% for 10, 30 for 25, etc -> that looks way better
    float trans = 1 - (log10(stabilityNormalized + 1) / log10(101));
    int transDecimal = trans * 100;

    if (transDecimal < 0) return 0;
    if (transDecimal > 100) return 100;

    return transDecimal;
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

    t->breaking_probability = CalculateBreakingProbabilty(pillar.sturdieness,
                                                          pillar.stability,
                                                          bullet.weight,
                                                          bullet.power);
    t->bullet_effects = bullet.effects;
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
