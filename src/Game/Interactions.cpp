#include "internal.h"

UiState ui;

void Action_PlaceTower()
{
    if (ui.ui_focus) return;

    if (ui.tower_placement_mode)
    {
        int xTile = mouseState.x / tileSize.width;
        int yTile = mouseState.y / tileSize.height;

        // TODO: -1 here? -> yes, because the pixel is the correct index
        // but the width is not
        int centerX = xTile * tileSize.width + (tileSize.width / 2 - 1);
        int centerY = yTile * tileSize.height + (tileSize.height / 2 - 1);

        // TODO: is placable functionality correctly
        int tileId = tileMap.GetTileId(mouseState.x, mouseState.y);

        if (tileId == 0)
        {
            Sprite towerSprite =
                ui.tower_a_selected ? sprites.tower_a : sprites.tower_b;
            CreateTowerEntity(centerX, centerY, towerSprite);
            PlayAudioFile(&audio.pop_lo, false, 90);
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
