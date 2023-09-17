#include "../internal.h"

ComponentStore components;
EntityStore entities;
TowerStore towers;
EnemyStore enemies;
ItemStore items;
CannonTypeStore cannons;
ProjectileStore projectiles;

void InitializeProjectileStore(int storeCount)
{
    projectiles.size = storeCount;
    projectiles.units = new Projectile[storeCount];
    projectiles.pool_index = 0;
    // Automatic pooling
    projectiles.pool_size = 0;
    memset(projectiles.units, 0, sizeof(Projectile) * storeCount);
}

void InitializeItemStorage(int storeCount)
{
    cannons.size = storeCount;
    cannons.units = new CannonType[storeCount];
    memset(cannons.units, 0, sizeof(CannonType) * storeCount);
}

void InitializeCannonTypeStorage(int storeCount)
{
    items.size = storeCount;
    items.units = new Item[storeCount];
    memset(items.units, 0, sizeof(Item) * storeCount);
}

void InitializeEnemyStorage(int storeCount)
{
    enemies.size = storeCount;
    enemies.units = new Enemy[storeCount];
    enemies.pool_index = 0;
    // using automatic pool size
    enemies.pool_size = 0;
    memset(enemies.units, 0, sizeof(Enemy) * storeCount);
}

void InitializeTowerStorage(int storeCount)
{
    towers.size = storeCount;
    towers.units = new Tower[storeCount];
    memset(towers.units, 0, sizeof(Tower) * storeCount);
}

void InitializeComponentStorage(int storeCount)
{
    components.size = storeCount;
    components.memory = new EntityComponents[storeCount];

    EntityComponents defaultValue = {};

    // initialize everything with a default value
    // else the defaults in the animators are overwritten!
    for (int i = 0; i < storeCount; i++)
    {
        memcpy(&components.memory[i], &defaultValue, sizeof(EntityComponents));
    }
}

void InitializeEntities(int storeCount)
{
    entities.size = storeCount;
    entities.units = new Entity[storeCount];
    memset(entities.units, 0, sizeof(Entity) * storeCount);

    // TODO: doesn't work with rendering -> because still gets found
    // crashes then because no sprite
    // InitEntityZero();

    InitializeComponentStorage(storeCount);
    InitializeTowerStorage(storeCount);
    InitializeEnemyStorage(storeCount);
    InitializeItemStorage(storeCount);
    InitializeCannonTypeStorage(storeCount);
    InitializeProjectileStore(storeCount);
}
