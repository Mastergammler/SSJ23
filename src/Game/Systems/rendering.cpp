#include "../internal.h"

const u32 DEBUG_COLOR = 0xFFE8FA;

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
        if (e.type == CRAFT_ITEM) continue;

        int drawStartX = e.x - (_tileSize.width / 2 - 1);
        int drawStartY = e.y - (_tileSize.height / 2 - 1);

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

void RenderCraftingItems(ScreenBuffer buffer)
{

    for (int i = 0; i < entities.unit_count; ++i)
    {
        Entity e = entities.units[i];

        if (e.type != CRAFT_ITEM) continue;

        int drawStartX = e.x - (_tileSize.width / 2 - 1);
        int drawStartY = e.y - (_tileSize.height / 2 - 1);

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

void Debug_DrawEntityMovementPossibilities(ScreenBuffer buffer)
{
    for (int i = 0; i < entities.unit_count; ++i)
    {
        Entity e = entities.units[i];

        if (e.type != ENEMY) continue;

        Tile enemyTile = *_tileMap.tileAt(e.x, e.y);

        if (enemyTile.adjacent & NORTH)
        {
            DrawLine(buffer, e.x, e.y, e.x, e.y + 12, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & SOUTH)
        {
            DrawLine(buffer, e.x, e.y - 12, e.x, e.y, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & EAST)
        {
            DrawLine(buffer, e.x, e.y, e.x + 12, e.y, DEBUG_COLOR);
        }
        if (enemyTile.adjacent & WEST)
        {
            DrawLine(buffer, e.x - 12, e.y, e.x, e.y, DEBUG_COLOR);
        }
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
                int spriteIdx =
                    cur->hovered ? cur->hover_sprite_index : cur->sprite_index;
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
