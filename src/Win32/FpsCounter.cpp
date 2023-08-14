#include "../imports.h"

struct FpsCounter
{
    int fps;
    float delta_time;
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

    bool Update()
    {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        delta_time = (float)(current_time.QuadPart - last_time.QuadPart) /
                     frequency.QuadPart;
        last_time = current_time;
        fps = 1 / delta_time;

        // check if it should display new value
        time_counter += delta_time;

        if (time_counter > fps_update_threshold)
        {
            time_counter -= fps_update_threshold;
            return true;
        }
        return false;
    }
};
