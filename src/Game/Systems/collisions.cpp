#include "../internal.h"

// can't return the const element directly, that's why we copy it here
// Not sure if that's the best handling, but ui relies on many places on finding
// an element -> so we want to prevent returning null here
UiElement nullElemCopy = UiElement{.visible = NullElement.visible,
                                   .initialized = NullElement.initialized,
                                   .id = NullElement.id,
                                   .layer = NullElement.layer,
                                   .on_click = NullElement.on_click};

UiElement* FindHighestLayerCollision(int x, int y)
{
    UiElement* foundElement = &nullElemCopy;
    for (int i = 0; i < uiElements.count; ++i)
    {
        UiElement* cur = &uiElements.elements[i];
        if (cur->visible)
        {
            if (cur->x_start <= x && cur->x_end >= x && cur->y_start <= y &&
                cur->y_end >= y)
            {
                if (cur->layer > foundElement->layer)
                {
                    foundElement = cur;
                }
            }
        }
    }

    return foundElement;
}

void ExecuteProjectileAction(int entityId)
{
    Entity e = entities.units[entityId];
    Projectile* p = &projectiles.units[e.storage_id];
    p->state = DESTROYED;
}

void ExecuteEnemyAction(int entityId, Projectile p)
{
    Entity e = entities.units[entityId];
    Enemy* en = &enemies.units[e.storage_id];

    assert(e.component_mask & EFFECT_COUNTER);

    FrameTimer* effectTimer = &components.memory[e.id].effect_counter;

    // if he is already hit, we don't need to do anything else
    // becaues in that state enemy is immune
    if (en->state >= IS_HIT) return;

    en->state = IS_HIT;

    if (e.component_mask & SHADER_ANIM)
    {
        // reset shader anim / or start it
        FrameTimer* anim = &components.memory[e.id].shader_anim;
        if (anim->finished)
        {
            anim->finished = false;
            anim->frame_index = 0;
            anim->time_since_last_frame = 0;
        }
    }

    // Enemies should always have the component
    assert(effectTimer);

    // I only allow single effects for now
    // bullets should also only have one effect to trigger?
    // maybe i can implement multi effect later
    if (p.effect_mask & EFFECT_SLOW)
    {
        effectTimer->tracking_value = en->speed;
        effectTimer->finished = false;
        effectTimer->time_since_last_frame = 0;
        effectTimer->looping = false;
        effectTimer->frame_index = -1;
        effectTimer->frame_count = 0;
        effectTimer->time_to_start = 3;
        effectTimer->time_after_end = 0;
        effectTimer->frames = NULL;

        en->speed *= 0.45;
        en->state = IS_SLOWED;
        FrameTimer* anim = &components.memory[en->entity_id].animator;
        // TODO: is this a 1:1 scaling? maybe not!
        anim->time_scale *= 0.45;

        // TODO: slow down animation as well
        // -> for that i have to change the sprite animator first
        // to also have a speed value
        // and also reset this one as well -> this is quite problematic, because
        // i have no good connection for start and end state, both happen just
        // because i know about this
        // but there should be one component who knows about both and knows what
        // to reset -> Not quite sure how to do this yet ...
    }
    else if (p.effect_mask & EFFECT_STUN)
    {
        effectTimer->tracking_value = en->speed;
        effectTimer->finished = false;
        effectTimer->time_since_last_frame = 0;
        effectTimer->looping = false;
        // use -1 beacause were just doing the start time
        effectTimer->frame_index = -1;
        effectTimer->frame_count = 0;
        effectTimer->time_to_start = 1.5;
        effectTimer->time_after_end = 0;
        effectTimer->frames = NULL;

        en->speed = 0;
        en->state = IS_STUNNED;
    }
}

void HandleProjectileCollisions()
{
    for (int i = 0; i < projectiles.unit_count; i++)
    {
        Projectile* p = &projectiles.units[i];
        if (p->state == DESTROYED) continue;

        Entity proE = entities.units[p->entity_id];
        if (~proE.component_mask & COLLIDER) continue;

        Collider projColl = components.memory[p->entity_id].collider;
        CenterRect rectP = CenterRect(proE.x,
                                      proE.y,
                                      projColl.width,
                                      projColl.height);

        for (int e = 0; e < enemies.unit_count; e++)
        {
            Enemy enemy = enemies.units[e];
            Entity eneE = entities.units[enemy.entity_id];
            if (~eneE.component_mask & COLLIDER) continue;

            Collider eneColl = components.memory[enemy.entity_id].collider;
            CenterRect rectE = CenterRect(eneE.x,
                                          eneE.y,
                                          eneColl.width,
                                          eneColl.height);

            bool xHitLeft = rectP.x1 > rectE.x1 && rectP.x1 < rectE.x2;
            bool xHitRight = rectP.x2 > rectE.x1 && rectP.x2 < rectE.x2;
            bool yHitBot = rectP.y1 > rectE.y1 && rectP.y1 < rectE.y2;
            bool yHitTop = rectP.y2 > rectE.y1 && rectP.y2 < rectE.y2;

            if ((xHitLeft && (yHitBot || yHitTop)) ||
                (xHitRight && (yHitBot || yHitTop)))
            {
                ExecuteProjectileAction(proE.id);
                ExecuteEnemyAction(eneE.id, *p);

                break;
            }
        }
    }
}
