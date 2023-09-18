#pragma once

#include "../module.h"

enum Components
{
    ANIMATOR = 0x1 << 0,
    COLLIDER = 0x1 << 1,
    SHADER_ANIM = 0x1 << 2,
    EFFECT_COUNTER = 0x1 << 3
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

struct Keyframe
{
    int index;
    float time_per_frame;
};

/**
 * A frame timer manages key frames
 */
struct FrameTimer
{
    bool looping;
    bool finished = true;
    Keyframe* frames;

    /**
     * pointer to the actual data the frames refer to
     * needs to be casted into the appropriate type
     * TEST: is this good to have it here?
     */
    void* data;

    /**
     * Size of a single element of the data
     * mostly for debugging purposes
     */
    int data_element_size;

    /**
     * value to track during the animation time
     * for resetting them after the animation has finished
     */
    float tracking_value;

    float time_since_last_frame;

    /**
     * Only affects the animation speed, not the start and end times
     */
    float time_scale = 1.;

    /**
     * Time until the animator starts counting frame time
     */
    float time_to_start;

    /**
     * Time after the last frame, on which the animation is considered finished
     */
    float time_after_end;

    int frame_count;
    /*
     * We start at frame -1, because of the wait till start functionality
     * When it's -1, it means we don't have started yet
     * TODO: this looks a bit akward, maybe change this?
     */
    int frame_index = -1;
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
    FrameTimer animator;
    Collider collider;
    FrameTimer shader_anim;
    /**
     * Timer for a effect to time out
     * = same as immunity counter?
     */
    FrameTimer effect_counter;
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
