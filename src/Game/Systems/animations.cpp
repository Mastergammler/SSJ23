#include "../components.h"
#include "../internal.h"

void AnimateEntities()
{
    for (int i = 0; i < entities.unit_count; ++i)
    {
        int component_mask = entities.units[i].component_mask;

        // TODO: refactor? not technically an animation
        if (component_mask & EFFECT_COUNTER)
        {
            FrameTimer* effectTimer = &components.memory[i].effect_counter;
            if (!effectTimer->finished)
            {
                int keyframe = NextKeyframeIndex(*effectTimer);
            }
            else if (entities.units[i].type == ENEMY)
            {
                Entity e = entities.units[i];
                Enemy* en = &enemies.units[e.storage_id];
                if (effectTimer->finished && en->state >= IS_HIT)
                {
                    // reset walking state
                    en->state = WALKING;
                    en->speed = effectTimer->tracking_value;
                }
            }
        }

        if (component_mask & ANIMATOR)
        {
            Animator* anim = &components.memory[i].animator;

            anim->time_since_last_sample += Time.sim_time;
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
        if (component_mask & SHADER_ANIM)
        {
            FrameTimer* timer = &components.memory[i].shader_anim;
            if (!timer->finished)
            {
                // TODO: do i need to handle the keyframe?
                int keyframe = NextKeyframeIndex(*timer);
            }
        }
    }
}
