#include "../module.h"

enum Components
{
    ANIMATOR = 0x1 << 0,
    COLLIDER = 0x1 << 1,
};

struct Collider
{
    /**
     * LR BT
     */

    int x_offset;
    int y_offset;
    int height;
    int width;
};

struct Animator
{
    bool looping;
    bool initialized;

    /**
     * time passed till last frame was displayed
     */
    float time_since_last_sample;

    /**
     * Animation speed
     */
    float time_per_sample;

    int sample_count;
    int sample_index;
    Sprite* samples;
};

struct EntityComponents
{
    Animator animator;
    Collider collider;
};

/**
 * Has to be managed synchronously with the entities
 * Should not be accessed without an entity?
 */
struct ComponentStore
{
    EntityComponents* memory;
    int size;
    int count;
};
