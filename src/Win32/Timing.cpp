#include "../imports.h"
#include <winnt.h>

struct Clock
{
    int warmup_iterations = 60;
    int current_iteration;

    int fps;
    int fps_min = 10000;
    int fps_max = 0;

    /**
     * Time since the last update call
     * When update is called every frame = frame time
     * Delta time is in seconds?
     * This is the actual passed time, so all IO system based things
     * should use this time
     */
    float delta_time_real;

    float time_scale = 1;

    /**
     * this is the scaled delta time (simulation time)
     * it depends on time scale
     * So all entities and game systems should use this time
     */
    float sim_time;

    float delta_min = 10000;
    float delta_max = 0;

    // TODO:  i would think this should be in microseconds as well
    //  but maybe i'm mistaken, because this seems to work correctly
    //  IT WORKS, WHY?
    float fps_update_threshold = 0.25;
    float time_counter;
    LARGE_INTEGER frequency;
    LARGE_INTEGER last_time;
    LARGE_INTEGER start_time;

    Clock()
    {
        // get the initial frequency (which stays constant)
        QueryPerformanceFrequency(&frequency);
        // set the initial value
        QueryPerformanceCounter(&last_time);
        QueryPerformanceCounter(&start_time);

        // initial value / can't be 0
        delta_time_real = 0.0166666666f;
        time_counter = delta_time_real;
    }

    void Update()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        delta_time_real = (float)(current_time.QuadPart - last_time.QuadPart) /
                          frequency.QuadPart;
        last_time = current_time;
        fps = 1 / delta_time_real;
        time_counter += delta_time_real;
        sim_time = time_scale * delta_time_real;

        if (current_iteration > warmup_iterations)
        {
            fps_max = fps > fps_max ? fps : fps_max;
            fps_min = fps < fps_min ? fps : fps_min;
            delta_max = delta_time_real > delta_max ? delta_time_real
                                                    : delta_max;
            delta_min = delta_time_real < delta_min ? delta_time_real
                                                    : delta_min;
        }
        else
        {
            current_iteration++;
        }
    }

    /**
     * checks the time since the last update call in ms
     */
    float CheckDeltaTimeMs()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        float deltaTime = (float)(current_time.QuadPart - last_time.QuadPart) /
                          frequency.QuadPart;
        return deltaTime * 1000.;
    }

    /**
     * Time since start (of the counter = program strat)
     * Rounds to the integer value
     */
    int CheckTimeSinceStart()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        float deltaTime = (float)(current_time.QuadPart - start_time.QuadPart) /
                          frequency.QuadPart;
        return (int)deltaTime * 1000;
    }

    bool ShowValue()
    {
        if (time_counter > fps_update_threshold)
        {
            time_counter -= fps_update_threshold;
            return true;
        }
        return false;
    }
};
