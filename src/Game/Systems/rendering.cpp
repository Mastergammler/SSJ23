#include "../internal.h"

void RenderTowers(ScreenBuffer buffer, v2 drawPos, Entity e)
{
    Tower t = towers.units[e.storage_id];

    if (t.state == TOWER_ACTIVE)
    {
        // should not be draw on lower border
        int yOffset = Game.tile_size.height / 2 - 2;

        // pillar
        DrawSprite(buffer, v2{drawPos.x, drawPos.y + yOffset}, t.pillar_sprite);
        int onTopOffset = yOffset +
                          Game.tile_size.height * t.pillar_sprite.y_tiles -
                          Game.tile_size.height / 2 + 2;
        // bullet
        DrawSprite(buffer,
                   v2{drawPos.x, drawPos.y + onTopOffset},
                   t.bullet_sprite);
    }
    else if (t.state == TOWER_BROKEN)
    {
        DrawSprite(buffer, drawPos, e.sprite);
    }
}

// TODO: REFACTOR - this get's too messy
void RenderEntities(ScreenBuffer buffer)
{
    // determine render order / TODO: is this performant enough? -> seems so
    vector<int> renderOrder(entities.unit_count);
    for (int i = 0; i < entities.unit_count; ++i)
    {
        renderOrder[i] = i;
    }

    sort(renderOrder.begin(), renderOrder.end(), [&](int a, int b) {
        return entities.units[a].y > entities.units[b].y;
    });

    for (int i = 0; i < entities.unit_count; ++i)
    {
        int renderOrderIndex = renderOrder[i];
        Entity e = entities.units[renderOrderIndex];

        // craft items are rendered separately, because they need to
        // be on top of the ui
        if (e.type == CRAFT_ITEM || e.type == TOWER_PROTO) continue;

        if (e.type == PROJECTILE)
        {
            Projectile p = projectiles.units[e.storage_id];
            if (p.state == DESTROYED) continue;
        }

        Sprite sprite;
        bool specialSprite = false;
        if (e.type == ENEMY)
        {
            Enemy enemy = enemies.units[e.storage_id];
            if (enemy.state == TARGET_LOCATION) continue;

            if (enemy.state == IS_STUNNED)
            {
                sprite = Res.sprites.enemy_frozen;
                specialSprite = true;
            }
        }

        int drawStartX = e.x - (Game.tile_size.width / 2 - 1);
        int drawStartY = e.y - (Game.tile_size.height / 2 - 1);
        v2 drawPos = {drawStartX, drawStartY};

        if (!specialSprite)
        {
            if (e.component_mask & ANIMATOR)
            {
                Animator anim = components.memory[e.id].animator;
                sprite = anim.samples[anim.sample_index];
            }
            else
            {
                sprite = e.sprite;
            }
        }

        Shader shader = {};
        if (e.component_mask & SHADER_ANIM)
        {
            FrameTimer anim = components.memory[e.id].shader_anim;
            // TODO: i don't think it's good that i have to check as
            // much stuff here
            //  this should be simpler!
            if (!anim.finished && anim.frame_index >= 0)
            {
                // TODO: still impacted by the issue with the index
                // for end time but i have to fix this in the
                // animation
                shader = Res.animations.enemy_hit.shaders[anim.frame_index];
            }
        }

        if (e.type == TOWER)
        {
            RenderTowers(buffer, drawPos, e);
        }
        else
        {
            DrawSprite(buffer, drawPos, sprite, shader);
        }
    }
}

void RenderEntitiesOfType(ScreenBuffer buffer, EntityType type)
{

    for (int i = 0; i < entities.unit_count; ++i)
    {
        Entity e = entities.units[i];

        if (e.type != type) continue;

        int drawStartX = e.x - (Game.tile_size.width / 2 - 1);
        int drawStartY = e.y - (Game.tile_size.height / 2 - 1);
        v2 drawPos = {drawStartX, drawStartY};

        Sprite sprite;
        if (e.component_mask & ANIMATOR)
        {
            Animator anim = components.memory[e.id].animator;
            sprite = anim.samples[anim.sample_index];
        }
        else
        {
            sprite = e.sprite;
        }

        DrawSprite(buffer, drawPos, sprite);
    }
}

void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet)
{
    for (int l = 0; l < uiElements.layer_count; ++l)
    {
        for (int i = 0; i < uiElements.count; ++i)
        {
            UiElement* cur = &uiElements.elements[i];
            if (!cur->visible || cur->layer != l) continue;

            if (cur->type == UI_PANEL)
            {
                DrawPanel(buffer,
                          cur->x_start,
                          cur->y_start,
                          sheet,
                          cur->sprite_index,
                          cur->x_tiles,
                          cur->y_tiles);
            }
            else
            {
                int spriteIdx = cur->hovered ? cur->hover_sprite_index
                                             : cur->sprite_index;
                DrawTiles(buffer,
                          cur->x_start,
                          cur->y_start,
                          sheet,
                          spriteIdx,
                          cur->x_tiles,
                          cur->y_tiles);
                // TODO: kind of a workaround, dunno how to do this
                // better atm
                cur->hovered = false;
            }
        }
    }
}
