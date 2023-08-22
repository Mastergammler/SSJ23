#include "module.h"
// TODO: this is now a direct reference, and not done via the module
//  this breaks the concept a bit, need a better solution
//  - probelm is the Sprite struct
#include "types.h"

enum EntityType
{
    TOWER = 0x0,
    ENEMY = 0x1,
    PROJECTILE = 0x10
};

enum Direction
{
    NORTH = 0,
    EAST = 90,
    SOUTH = 180,
    WEST = 270
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
