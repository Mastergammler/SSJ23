#include "../internal.h"
#include "../ui.h"

UiState ui;

void Action_SpawnEnemy()
{
    if (ui.show_crafting_panels) return;

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
    ui.show_crafting_panels = !ui.show_crafting_panels;
    UiElement* panel = &uiElements.elements[ui.tower_crafting_panel_id];
    UiElement* items = &uiElements.elements[ui.items_panel_id];
    panel->visible = ui.show_crafting_panels;
    items->visible = ui.show_crafting_panels;

    // hide child elements
    for (int i = 0; i < uiElements.count; ++i)
    {
        UiElement* el = &uiElements.elements[i];
        if (el->parent_id == items->id)
        {
            el->visible = items->visible;
        }
        else if (el->parent_id == panel->id)
        {
            el->visible = panel->visible;
        }
    }
}

void Action_StartGame()
{
    UiElement* startBtn = &uiElements.elements[ui.start_game_button_id];
    UiElement* mapsPanel = &uiElements.elements[ui.map_selection_panel_id];
    UiElement* exitBtn = &uiElements.elements[ui.exit_game_button_id];

    UiElement* towerSelection =
        &uiElements.elements[ui.tower_selection_panel_id];
    UiElement* tmp1 = &uiElements.elements[ui.tmp_1];
    UiElement* tmp2 = &uiElements.elements[ui.tmp_2];

    startBtn->visible = false;
    mapsPanel->visible = false;
    exitBtn->visible = false;

    towerSelection->visible = true;
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

void Action_StartDrag()
{
    if (!ui.is_dragging)
    {
        ui.is_dragging = true;

        UiElement* el = ui.ui_focus_element;
        ItemPanelPositionMap slotEntityMap = ui.ui_entity_map[el->id];

        ui.dragged_entity_id = slotEntityMap.entity_id;
    }
}

void Action_DoDragging()
{
    if (ui.is_dragging)
    {
        Entity* e = &entities.units[ui.dragged_entity_id];

        e->x = mouseState.x;
        e->y = mouseState.y;
    }
}

// TODO: nodo need to cleanup the ui
//  need a better tracking system
//  some way to distinguish between drop and non drops
//
// TODO: Refactor UI state system
// -> its driving me crazy right now, i don't find stuff anymore
// -> introduce flags for ui items -> then i can add drop_source and drop_target
// -> And do interactions based on that -> and can remove onclick?
// -> Or set onClick differently for the build buttons

// FIXME: something is dropped some resetting happening
// -> when i move something twice
// -> Because i have no way of distinguishing starts and ends only
// => Could just do another enum?
void Action_Drop()
{

    Entity* e = &entities.units[ui.dragged_entity_id];
    UiElement* dropTarget = ui.ui_focus_element;

    int initialSlotId = -1;
    for (const auto& pair : ui.ui_entity_map)
    {
        if (pair.second.entity_id == e->id)
        {
            initialSlotId = pair.second.initial_slot_id;
            break;
        }
    }

    // entity has to have been tracked somewhere
    assert(initialSlotId != -1);

    // TODO: use zero element?
    // This is wrong, because it must be a valid drop target ...
    // -> only certain elements allowed
    if (dropTarget->type == UI_DRAG_DROP)
    {
        ItemPanelPositionMap mapping = ui.ui_entity_map[dropTarget->id];

        // is default -> empty slot
        if (mapping.initial_slot_id == 0)
        {
            Position slotCenter = ItemSlotCenter(dropTarget->id);
            e->x = slotCenter.x;
            e->y = slotCenter.y;

            ItemPanelPositionMap mapping = ui.ui_entity_map[initialSlotId];
            ui.ui_entity_map.erase(initialSlotId);

            // initial slot stays same, for reset
            mapping.current_slot_id = dropTarget->id;
            ui.ui_entity_map[dropTarget->id] = mapping;
        }
        // switch entities
        else
        {
            // todo switch stuff
        }
    }
    else
    {
        // reset to initial position
        Position slotCenter = ItemSlotCenter(initialSlotId);
        e->x = slotCenter.x;
        e->y = slotCenter.y;
    }

    // TODO: do we have 0 entity?
    ui.dragged_entity_id = EntityZero.id;
    ui.is_dragging = false;
}
