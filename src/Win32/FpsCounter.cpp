#include "../imports.h"

struct FpsCounter
{
    int fps;

    /**
     * Time since the last update call
     * When update is called every frame = frame time
     * Delta time is in seconds?
     */
    float delta_time;

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
        delta_time = 0.0166666666f;
        time_counter = delta_time;
    }

    void Update()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        delta_time = (float)(current_time.QuadPart - last_time.QuadPart) /
                     frequency.QuadPart;
        last_time = current_time;
        fps = 1 / delta_time;
        time_counter += delta_time;
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
