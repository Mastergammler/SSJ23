#include "../entities.h"
#include "../internal.h"
#include "../ui.h"

void SimulateTower()
{
    for (int i = 0; i < towers.unit_count; i++)
    {
        Tower* t = &towers.units[i];
        if (t->state == TOWER_BROKEN) continue;

        // update shot timer
        t->time_since_last_shot += Time.sim_time;
        if (t->time_since_last_shot > t->fire_rate)
        {
            Tile* targetTile = NULL;
            for (int p = 0; p < t->tile_count; p++)
            {
                Tile* tile = t->relevant_tiles[p];
                if (tile->tracker->entity_count > 0)
                {
                    targetTile = tile;
                    break;
                }
            }

            // found sb to shoot at
            if (targetTile)
            {
                t->time_since_last_shot = 0;

                Entity tower = entities.units[t->entity_id];

                // TODO: create projectile
                int projectile = CreateProjectileEntity();
                Entity* p = &entities.units[projectile];
                p->sprite = t->bullet_sprite;
                p->x = tower.x;
                p->y = tower.y;
                p->move_x = tower.x;
                p->move_y = tower.y;

                Position tileCenterPos = TileCenterPosition(*targetTile);
                Projectile* proj = &projectiles.units[p->storage_id];
                proj->state = ACTIVE;
                proj->target_x = tileCenterPos.x;
                proj->target_y = tileCenterPos.y;
                proj->speed = t->bullet_speed;

                // TODO: animations and sounds etc;

                // if rnd is smaller than the probability of breaking then we
                // break breaking probability of 0 means rnd will never be lower
                // than it breaking probability 10 = 1/10 chance of hitting that
                // number etc
                int rnd = RndInt(100);
                if (rnd < t->breaking_probability)
                {
                    t->state = TOWER_BROKEN;
                    // TODO: trigger animation event etc

                    // Logf("Tower breaking No %d, probablity %d",
                    //     rnd,
                    //     t->breaking_probability);
                }
            }
        }
    }
}

// TODO: another usecase for animation system? Not quite right?
struct SpawnTimer
{
    float start_delay = 5;
    int time_per_wave = 5;
    float time_since_last_wave = time_per_wave - start_delay;
    float time_since_last_enemy = 0;
    float enemy_spacing = 0.75;
    float spawn_count = 1;
    int current_wave_spawn_count = 0;
};

SpawnTimer spawn;

void SpawnEnemy()
{
    PlaySoundEffect(&Res.audio.pop_hi);

    assert(Game.tile_map.spawn_count > 0);
    int spawnIndex = RndInt(Game.tile_map.spawn_count - 1);
    Tile spawnTile = *Game.tile_map.spawns[spawnIndex];

    v2 centerPos = TileCenterPosition(spawnTile);

    // TODO: real direction and speed?
    CreateEnemyEntity(centerPos.x,
                      centerPos.y,
                      Res.sprites.enemy_a,
                      SOUTH,
                      20,
                      Res.animations.enemy_anim,
                      Res.animations.enemy_hit);
}

void SpawningSystem()
{
    spawn.time_since_last_wave += Time.sim_time;

    if (spawn.time_since_last_wave > spawn.time_per_wave)
    {
        spawn.time_since_last_enemy += Time.sim_time;
        if (spawn.time_since_last_enemy > spawn.enemy_spacing)
        {
            spawn.time_since_last_enemy -= spawn.enemy_spacing;
            SpawnEnemy();
            spawn.current_wave_spawn_count++;
        }
        // all have spawned
        if (spawn.current_wave_spawn_count >= spawn.spawn_count)
        {
            spawn.time_since_last_enemy = 0;
            spawn.time_since_last_wave = 0;
            spawn.current_wave_spawn_count = 0;
            spawn.spawn_count *= 1.5;
        }
    }
}
