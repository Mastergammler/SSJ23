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
                int keyframe = NextKeyframeIndex(effectTimer);
            }
            else if (entities.units[i].type == ENEMY)
            {
                // TODO: this is super distributed logic
                // how would i know how this is connected to the other logic?
                Entity e = entities.units[i];
                Enemy* en = &enemies.units[e.storage_id];
                if (effectTimer->finished && en->state >= IS_HIT)
                {
                    // reset walking state
                    en->state = WALKING;
                    en->speed = effectTimer->tracking_value;

                    FrameTimer* anim = &components.memory[en->entity_id].animator;
                    // TODO: inflecible, doesn't reset to the original value
                    anim->time_scale = 1;
                }
            }
        }

        if (component_mask & ANIMATOR)
        {
            FrameTimer* anim = &components.memory[i].animator;
            if (!anim->finished)
            {
                NextKeyframeIndex(anim);
            }
        }

        if (component_mask & SHADER_ANIM)
        {
            FrameTimer* timer = &components.memory[i].shader_anim;
            if (!timer->finished)
            {
                NextKeyframeIndex(timer);
            }
        }
    }
}
