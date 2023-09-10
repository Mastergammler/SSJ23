#include "../entities.h"
#include "../internal.h"

int CreateEnemy(int entityId, int speed)
{
    assert(enemies.unit_count < enemies.size);

    int id = enemies.unit_count++;
    Enemy* e = &enemies.units[id];

    e->storage_id = id;
    e->entity_id = entityId;

    e->speed = speed;
    e->state = WALKING;

    e->initialized = true;

    return e->storage_id;
}

// TODO: how to put the animation data in there?
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation,
                      ShaderAnimation shaderAnimation)
{
    Entity* e = InitNextEntity();
    e->storage_id = CreateEnemy(e->id, speed);

    e->x = x;
    e->y = y;
    e->move_x = x;
    e->move_y = y;
    e->type = ENEMY;
    e->direction = direction;
    e->sprite = sprite;

    // TODO: should i initialize the components differently? Not together with
    // the entity?
    e->component_mask = (ANIMATOR | COLLIDER | SHADER_ANIM);
    Animator* anim = &components.memory[e->id].animator;
    anim->initialized = true;
    anim->looping = true;
    anim->sample_index = 0;
    anim->sample_count = animation.sprite_count;
    anim->time_per_sample = animation.time_per_sprite;
    anim->samples = animation.sprites;

    Collider* coll = &components.memory[e->id].collider;
    coll->x_offset = 0;
    coll->y_offset = 0;
    coll->height = 12;
    coll->width = 12;

    FrameTimer* shaderAnim = &components.memory[e->id].shader_anim;
    shaderAnim->looping = false;
    shaderAnim->frames = shaderAnimation.keyframes;
    shaderAnim->frame_count = shaderAnimation.frame_count;

    return e->id;
}
