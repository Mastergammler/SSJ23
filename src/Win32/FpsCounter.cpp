#include "../imports.h"

struct FpsCounter
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
     */
    float frame_delta_time;

    float delta_min = 10000;
    float delta_max = 0;

    // TODO:  i would think this should be in microseconds as well
    //  but maybe i'm mistaken, because this seems to work correctly
    //  IT WORKS, WHY?
    float fps_update_threshold = 0.25;
    float time_counter;
    LARGE_INTEGER frequency;
    LARGE_INTEGER last_time;

    FpsCounter()
    {
        // get the initial frequency (which stays constant)
        QueryPerformanceFrequency(&frequency);
        // set the initial value
        QueryPerformanceCounter(&last_time);

        // initial value / can't be 0
        frame_delta_time = 0.0166666666f;
        time_counter = frame_delta_time;
    }

    void Update()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        frame_delta_time = (float)(current_time.QuadPart - last_time.QuadPart) /
                     frequency.QuadPart;
        last_time = current_time;
        fps = 1 / frame_delta_time;
        time_counter += frame_delta_time;

        if (current_iteration > warmup_iterations)
        {
            fps_max = fps > fps_max ? fps : fps_max;
            fps_min = fps < fps_min ? fps : fps_min;
            delta_max = frame_delta_time > delta_max ? frame_delta_time : delta_max;
            delta_min = frame_delta_time < delta_min ? frame_delta_time : delta_min;
        }
        else { current_iteration++; }
    }

    float CheckDeltaTimeMs()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        float deltaTime = (float)(current_time.QuadPart - last_time.QuadPart) /
                          frequency.QuadPart;
        return deltaTime * 1000.;
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
