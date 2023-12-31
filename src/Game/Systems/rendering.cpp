#include "../internal.h"

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

        // craft items are rendered separately, because they need to be on top
        // of the ui
        if (e.type == CRAFT_ITEM || e.type == TOWER_PROTO) continue;

        if (e.type == PROJECTILE)
        {
            Projectile p = projectiles.units[e.storage_id];
            if (p.state == DESTROYED) continue;
        }

        int drawStartX = e.x - (Game.tile_size.width / 2 - 1);
        int drawStartY = e.y - (Game.tile_size.height / 2 - 1);

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

        if (e.type == TOWER)
        {
            Tower t = towers.units[e.storage_id];

            // should not be draw on lower border
            int yOffset = Game.tile_size.height / 2 - 2;
            // pillar
            DrawTiles(buffer,
                      drawStartX,
                      drawStartY + yOffset,
                      *t.pillar_sprite.sheet,
                      t.pillar_sprite.sheet_start_index,
                      t.pillar_sprite.x_tiles,
                      t.pillar_sprite.y_tiles);

            int onTopOffset = yOffset +
                              Game.tile_size.height * t.pillar_sprite.y_tiles -
                              Game.tile_size.height / 2 + 2;
            // bullet
            DrawTiles(buffer,
                      drawStartX,
                      drawStartY + onTopOffset,
                      *t.bullet_sprite.sheet,
                      t.bullet_sprite.sheet_start_index,
                      t.bullet_sprite.x_tiles,
                      t.bullet_sprite.y_tiles);
        }
        else
        {
            DrawTiles(buffer,
                      drawStartX,
                      drawStartY,
                      *sprite.sheet,
                      sprite.sheet_start_index,
                      sprite.x_tiles,
                      sprite.y_tiles);
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

        DrawTiles(buffer,
                  drawStartX,
                  drawStartY,
                  *sprite.sheet,
                  sprite.sheet_start_index,
                  sprite.x_tiles,
                  sprite.y_tiles);
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
                // TODO: kind of a workaround, dunno how to do this better atm
                cur->hovered = false;
            }
        }
    }
}
