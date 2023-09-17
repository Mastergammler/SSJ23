#include "../internal.h"

// TODO: don't return the index, makes no sense
//- change values on the animator
int NextKeyframeIndex(FrameTimer& anim)
{
    if (anim.frame_count > 0)
    {
        assert(anim.frames);
    }

    anim.time_since_last_frame += Time.sim_time;

    // case not started yet
    if (anim.frame_index == -1)
    {
        if (anim.time_since_last_frame > anim.time_to_start)
        {
            anim.time_since_last_frame -= anim.time_to_start;
            anim.frame_index++;
        }
    }
    else if (anim.frame_index >= anim.frame_count)
    {
        // TODO: this looks very wanky, this is probably not the best way
        if (anim.time_since_last_frame < anim.time_after_end)
        {
            // return last frame
            return anim.frame_count - 1;
        }
        else
        {
            anim.finished = true;
            return -1;
        }
    }
    else
    {
        Keyframe displayedFrame = anim.frames[anim.frame_index];
        float scaledFrameTime = displayedFrame.time_per_frame / anim.time_scale;

        if (anim.time_since_last_frame > scaledFrameTime)
        {
            anim.time_since_last_frame -= scaledFrameTime;
            anim.frame_index++;

            if (anim.frame_index >= anim.frame_count)
            {
                // TODO: this together with the other elseif branch seems quite
                // messy is there a better way to do this?
                if (anim.looping)
                {
                    anim.frame_index = 0;
                }
                else
                {
                    // return last frame, don't return the index, because it's
                    // not correct
                    return anim.frame_count - 1;
                }
            }
        }
    }

    return anim.frame_index;
}
