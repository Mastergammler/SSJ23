#include "../internal.h"
#include "../ui.h"

void SimulateTower()
{
    for (int i = 0; i < towers.unit_count; i++)
    {
        Tower* t = &towers.units[i];

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
            }
        }
    }
}

float timeToFirstWave = 15;
int timePerWave = 5;
float timeSinceLastWave = timePerWave - timeToFirstWave;
float timeSinceLastEnemy = 0;
float enemyDelay = 0.75;
float enemyAmount = 1;
int enemiesSpawnedThisWave = 0;

void SpawningSystem()
{
    timeSinceLastWave += Time.sim_time;

    if (timeSinceLastWave > timePerWave)
    {
        timeSinceLastEnemy += Time.sim_time;
        if (timeSinceLastEnemy > enemyDelay)
        {
            timeSinceLastEnemy -= enemyDelay;
            Action_SpawnEnemy();
            enemiesSpawnedThisWave++;
        }
        // all have spawned
        if (enemiesSpawnedThisWave >= enemyAmount)
        {
            timeSinceLastEnemy = 0;
            timeSinceLastWave = 0;
            enemiesSpawnedThisWave = 0;
            enemyAmount *= 1.5;
        }
    }
}
