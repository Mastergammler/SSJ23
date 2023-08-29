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

void ExecuteEnemyAction(int entityId)
{
    Entity e = entities.units[entityId];
    Enemy* en = &enemies.units[e.storage_id];

    if (en->state != IS_HIT)
    {
        en->state = IS_HIT;
        en->speed /= 2;
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
                ExecuteEnemyAction(eneE.id);
                // first collision only -> then the projectile is used
                // Logf("Projectile at %d,%d hitting enemy at %d,%d",
                //     proE.x,
                //     proE.y,
                //     eneE.x,
                //     eneE.y);
                break;
            }
        }
    }
}
