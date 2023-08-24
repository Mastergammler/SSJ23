#include "../internal.h"

UiState ui;

void Action_SpawnEnemy()
{
    if (ui.show_crafting_panel) return;

    Tile startTile = *_tileMap.spawns[0];

    int invertedY = _tileMap.rows - startTile.y - 1;
    int spawnX = startTile.x * _tileSize.width + _tileSize.width / 2 - 1;
    int spawnY = invertedY * _tileSize.height + _tileSize.height / 2 - 1;

    // TODO: get direction for real
    //  where to get the real speed from?
    CreateEnemyEntity(spawnX,
                      spawnY,
                      _sprites.enemy_a,
                      SOUTH,
                      20,
                      _animations.enemy_anim);
}

void Action_PlaceTower()
{
    if (ui.ui_focus) return;

    if (ui.tower_placement_mode)
    {
        Tile* tile = _tileMap.tileAt(mouseState.x, mouseState.y);

        int centerX = tile->x * _tileSize.width + (_tileSize.width / 2 - 1);
        int centerY = MirrorY(tile->y, _tileMap.rows) * _tileSize.height +
                      (_tileSize.height / 2 - 1);

        if (tile->tile_id == GRASS_TILE && !tile->is_occupied)
        {
            Sprite towerSprite =
                ui.tower_a_selected ? _sprites.tower_a : _sprites.tower_b;
            CreateTowerEntity(centerX, centerY, towerSprite);
            PlayAudioFile(&_audio.pop_lo, false, 90);

            tile->is_occupied = true;
        }
    }
}

void Action_ToggleTowerPreview()
{
    // 3 way switch
    if (ui.tower_placement_mode)
    {
        if (ui.tower_a_selected)
        {
            ui.tower_a_selected = !ui.tower_a_selected;
        }
        else
        {
            ui.tower_placement_mode = !ui.tower_placement_mode;
        }
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

void Action_StartGame()
{
    UiElement* startBtn = &uiElements.elements[ui.start_game_button_id];
    UiElement* mapsPanel = &uiElements.elements[ui.map_selection_panel_id];
    UiElement* exitBtn = &uiElements.elements[ui.exit_game_button_id];

    UiElement* tmp1 = &uiElements.elements[ui.tmp_1];
    UiElement* tmp2 = &uiElements.elements[ui.tmp_2];

    startBtn->visible = false;
    mapsPanel->visible = false;
    exitBtn->visible = false;

    tmp1->visible = true;
    tmp2->visible = true;

    navigation.in_menu = false;
    navigation.in_game = true;

    // TODO: trigger transition animation
    if (_audio.music.loaded)
    {
        // PlayAudioFile(&_audio.music, true, 80);
    }
}
