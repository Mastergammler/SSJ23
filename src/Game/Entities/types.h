#include "../Loading/types.h"
#include "../module.h"

enum EntityType
{
    TOWER = 0x0,
    ENEMY = 0x1,
    PROJECTILE = 0x10,
    CRAFT_ITEM = 0x11,
    TOWER_PROTO = 0x100,
};

/**
 * Direction / Orientation of the player
 * Anchored at Top
 */
enum Direction
{
    NORTH = 0b1000 << 4,
    EAST = 0b0010 << 4,
    SOUTH = 0b0001 << 4,
    WEST = 0b0100 << 4
};

enum EntityState
{
    WALKING,
    IS_HIT,
    TARGET_LOCATION
};

enum TowerState
{
    TOWER_BROKEN = 0x0,
    TOWER_ACTIVE = 0x1
};

/**
 * Entity information.
 * Information that other entities most likely would want to know
 */
struct Entity
{
    bool initialized;

    int id;
    int storage_id;

    /**
     * Center based x coordinate
     */
    int x;
    /**
     * Center based y coordinate
     * Anchored Bottom
     */
    int y;

    /**
     * Coordinates for movement calculation
     * Because they are to tiny for pixel movements
     */
    float move_x;
    float move_y;

    int component_mask;

    Sprite sprite;
    Direction direction;
    EntityType type;
};

struct EntityStore
{
    Entity* units;
    int unit_count;
    int size;
};

/**
 * Type specific information
 * mostly status information
 * Things that other entities don't need to know
 * most of the time.
 *
 * If they still do, it can be looked up
 */
struct Tower
{
    bool initialized;

    int entity_id;
    int storage_id;

    /**
     * in tiles
     */
    int radius;

    /**
     * how many times to fire per second
     */
    float fire_rate;

    /**
     * time in ms?
     */
    float time_since_last_shot;

    /**
     * How fast the bullet is supposed to travel
     * in tiles/s?
     */
    float bullet_speed;

    /**
     * Probability that after the next shot the tower still stands
     * Inversed: 0 = breaks, 100 = never breaks
     */
    int breaking_probability;

    // TODO: new values
    //  - state, is broken etc
    //  - sprite for is broken (change entity sprite?)
    Sprite bullet_sprite;
    Sprite pillar_sprite;

    TowerState state;

    /**
     * Tiles that the tower checks for attacking
     * The tile tracks what units are on it at every moment
     * And then the tower knows where or who to attack
     *
     * Links to the original tile in the tile map
     */
    Tile** relevant_tiles;
    int tile_count;
};

struct TowerStore
{
    Tower* units;
    int unit_count;
    int size;
};

struct Enemy
{
    bool initialized;

    int entity_id;
    int storage_id;

    int state;
    int speed;

    Tile* target;
};

struct EnemyStore
{
    Enemy* units;
    int unit_count;
    int size;
};

struct Item
{
    bool initialized;

    int entity_id;
    int storage_id;

    float weight;
    float aero;
    float stability;
    float sturdieness;
    float power;

    int effects;

    int bullet_sprite_idx;
    int pillar_sprite_idx;

    // TODO: how to actually map these?
    int shoot_sound_idx;
    int hit_sound_idx;
};

struct ItemStore
{
    Item* units;
    int unit_count;
    int size;
};

enum ProjectileState
{
    DESTROYED = 0x0,
    ACTIVE = 0x1,
};

// TODO: everything i need
struct Projectile
{
    int entity_id;
    int storage_id;

    int target_x;
    int target_y;

    int effect_mask;
    float speed;

    ProjectileState state;

    // hit sound here?
    int sound_idx;
};

struct ProjectileStore
{

    Projectile* units;
    int unit_count;
    int size;

    /**
     * Index for the position of the pool
     * This resets after the end of the and tries to reuse the first units
     */
    int pool_index;
    int pool_size;
};

struct CannonType
{
    bool initialized;

    int entity_id;
    int storage_id;

    int bullet_item_id;
    int pillar_item_id;

    /**
     * From 0-100
     * 100 - instantly collapses on build
     * 0 - never collapses
     */
    int breaking_probability;

    /**
     * In tiles, cross first, rounded corners -1
     */
    float range;
    /**
     * Shot's per second
     */
    float fire_rate;

    // TODO: what to do about this?
    float accuracy;

    /**
     * Tiles per s (diagonal is slower / normalized)
     */
    float bullet_speed;
};

struct CannonTypeStore
{
    CannonType* units;
    int unit_count;
    int size;
};
