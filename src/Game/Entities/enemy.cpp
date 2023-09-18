#include "../entities.h"
#include "../internal.h"

int CreateNewEnemy(int entityId, int speed)
{
    assert(enemies.unit_count < enemies.size);

    int id = enemies.unit_count++;
    Enemy* e = &enemies.units[id];

    e->storage_id = id;
    e->entity_id = entityId;

    e->speed = speed;
    e->state = WALKING;

    // TODO: does this need more validation?
    int targetIndex = RndInt(Game.tile_map.target_count - 1);
    e->target = Game.tile_map.targets[targetIndex];

    e->initialized = true;

    return e->storage_id;
}

void SetEnemyComponents(Entity* e,
                        SpriteAnimation animation,
                        ShaderAnimation shaderAnimation)
{
    // TODO: should i initialize the components differently? Not together with
    // the entity?
    e->component_mask = (ANIMATOR | COLLIDER | SHADER_ANIM | EFFECT_COUNTER);

    FrameTimer* anim = &components.memory[e->id].animator;
    anim->looping = true;
    anim->finished = false;
    anim->frame_index = -1;
    anim->time_since_last_frame = 0;
    anim->frame_count = animation.sprite_count;
    anim->data = animation.sprites;
    anim->data_element_size = sizeof(Sprite);
    anim->frames = animation.keyframes;

    Collider* coll = &components.memory[e->id].collider;
    coll->x_offset = 0;
    coll->y_offset = 0;
    coll->height = 12;
    coll->width = 12;

    FrameTimer* shaderAnim = &components.memory[e->id].shader_anim;
    shaderAnim->looping = false;
    shaderAnim->frames = shaderAnimation.keyframes;
    shaderAnim->frame_count = shaderAnimation.frame_count;

    FrameTimer* effectTimer = &components.memory[e->id].effect_counter;
    effectTimer->finished = true;
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
    Entity* e = NULL;
    if (enemies.pool_size <= enemies.unit_count && enemies.pool_size > 0)
    {
        // search for 10 matches, else create new one
        for (int i = 0; i < 10; i++)
        {
            Enemy* enemy = &enemies.units[enemies.pool_index++];

            // TODO: abstract this -> poolable smth struct?
            if (enemies.pool_index >= enemies.pool_size)
            {
                enemies.pool_index = 0;
            }

            if (enemy->state == TARGET_LOCATION)
            {
                e = &entities.units[enemy->entity_id];
                enemy->state = WALKING;
                break;
            }
        }
    }

    if (!e)
    {
        // increase the pool size for each new enemy we create
        // that way we can always use all the enemies in a pool
        // and not miss any of them
        enemies.pool_size++;
        assert(enemies.pool_size <= enemies.size);

        e = InitNextEntity();
        e->storage_id = CreateNewEnemy(e->id, speed);
    }

    e->x = x;
    e->y = y;
    e->move_x = x;
    e->move_y = y;
    e->type = ENEMY;
    e->direction = direction;
    e->sprite = sprite;

    SetEnemyComponents(e, animation, shaderAnimation);

    return e->id;
}
