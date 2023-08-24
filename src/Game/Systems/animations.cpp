#include "../internal.h"

void AnimateEntities()
{
    for (int i = 0; i < entities.unit_count; ++i)
    {
        int component_mask = entities.units[i].component_mask;
        if (component_mask & ANIMATOR)
        {
            Animator* anim = &components.memory[i].animator;

            anim->time_since_last_sample += measure.frame_delta_time;
            if (anim->time_since_last_sample > anim->time_per_sample)
            {
                // play next frame
                anim->sample_index++;
                anim->time_since_last_sample -= anim->time_per_sample;
            }

            if (anim->looping)
            {
                // reset for the loop
                if (anim->sample_index >= anim->sample_count)
                {
                    anim->sample_index = 0;
                }
            }
            else
            {
                // animation ended, correct for index overshoot
                // TODO: what happens when it ends? revert back to default
                // sprite?
                if (anim->sample_index >= anim->sample_count - 1)
                {
                    // set to last frame
                    anim->sample_index = anim->sample_count - 1;
                }
            }
        }
    }
}
