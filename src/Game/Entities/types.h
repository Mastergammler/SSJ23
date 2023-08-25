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

    int radius;
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

    int speed;
};

struct EnemyStore
{
    Enemy* units;
    int unit_count;
    int size;
};
