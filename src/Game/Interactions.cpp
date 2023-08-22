#include "internal.h"

UiState ui;

void Action_PlaceTower()
{
    if (ui.ui_focus) return;

    if (ui.tower_placement_mode)
    {
        Tile* tile = tileMap.tileAt(mouseState.x, mouseState.y);

        int centerX = tile->x * tileSize.width + (tileSize.width / 2 - 1);
        int centerY = MirrorY(tile->y, tileMap.rows) * tileSize.height +
                      (tileSize.height / 2 - 1);

        if (tile->tile_id == 0 && !tile->is_occupied)
        {
            Sprite towerSprite =
                ui.tower_a_selected ? sprites.tower_a : sprites.tower_b;
            CreateTowerEntity(centerX, centerY, towerSprite);
            PlayAudioFile(&audio.pop_lo, false, 90);

            tile->is_occupied = true;
        }
    }
}

void Action_ToggleTowerPreview()
{
    // 3 way switch
    if (ui.tower_placement_mode)
    {
        if (ui.tower_a_selected) { ui.tower_a_selected = !ui.tower_a_selected; }
        else { ui.tower_placement_mode = !ui.tower_placement_mode; }
    }
    else
    {
        ui.tower_placement_mode = true;
        ui.tower_a_selected = true;
    }
}

void Action_ToggleCraftingPanel()
{
    ui.show_crafting_panel = !ui.show_crafting_panel;
    UiElement* panel = &uiElements.elements[ui.crafting_panel_id];
    panel->visible = ui.show_crafting_panel;
}
