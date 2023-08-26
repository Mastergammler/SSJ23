#include "../../Win32/module.h"
#include "../internal.h"
#include "../systems.h"
#include "../ui.h"

UiState ui;
MouseState mouseState;
Navigation navigation;

void UpdateMouseState()
{
    mouseState.left_clicked = false;
    mouseState.left_released = false;
    mouseState.right_clicked = false;
    mouseState.right_released = false;

    mouseState.x = mouse.x;
    mouseState.y = mouse.y;

    bool leftClickedNew = mouse.buttons & MOUSE_LEFT;
    bool rightClickedNew = mouse.buttons & MOUSE_RIGHT;

    if (leftClickedNew != mouseState.left_down)
    {
        if (leftClickedNew)
            mouseState.left_clicked = true;
        else
            mouseState.left_released = true;
    }

    if (rightClickedNew != mouseState.right_down)
    {
        if (rightClickedNew)
            mouseState.right_clicked = true;
        else
            mouseState.right_released = true;
    }

    mouseState.left_down = leftClickedNew;
    mouseState.right_down = rightClickedNew;
}

void ProcessMouseActions()
{
    UiElement* hovered = FindHighestLayerCollision(mouseState.x, mouseState.y);
    hovered->hovered = true;
    ui.ui_focus = hovered->id != -1;
    ui.ui_focus_element = hovered;

    if (mouseState.left_clicked)
    {
        hovered->on_click();
        Action_PlaceTower();
    }

    if (mouseState.left_released)
    {
        if (ui.is_dragging)
        {
            Action_Drop();
        }
    }

    if (mouseState.left_down)
    {
        if (ui.is_dragging)
        {
            Action_DoDragging();
        }
    }

    if (mouseState.right_clicked)
    {
        Action_ToggleTowerPreview();
    }
}

void Action_SpawnEnemy()
{
    if (ui.crafting.visible) return;

    PlayAudioFile(&Res.audio.pop_hi, false, 90);
    Tile startTile = *_tileMap.spawns[0];

    int invertedY = _tileMap.rows - startTile.y - 1;
    int spawnX = startTile.x * _tileSize.width + _tileSize.width / 2 - 1;
    int spawnY = invertedY * _tileSize.height + _tileSize.height / 2 - 1;

    // TODO: get direction for real
    //  where to get the real speed from?
    CreateEnemyEntity(spawnX,
                      spawnY,
                      Res.sprites.enemy_a,
                      SOUTH,
                      20,
                      Res.animations.enemy_anim);
}

void Action_PlaceTower()
{
    if (ui.ui_focus) return;

    if (ui.placement.active)
    {
        Tile* tile = _tileMap.tileAt(mouseState.x, mouseState.y);

        int centerX = tile->x * _tileSize.width + (_tileSize.width / 2 - 1);
        int centerY = MirrorY(tile->y, _tileMap.rows) * _tileSize.height +
                      (_tileSize.height / 2 - 1);

        if (tile->tile_id == GRASS_TILE && !tile->is_occupied)
        {
            Sprite towerSprite = ui.placement.tower_a_selected ? Res.sprites.tower_a
                                                               : Res.sprites.tower_b;
            CreateTowerEntity(centerX, centerY, towerSprite);
            PlayAudioFile(&Res.audio.pop_lo, false, 90);

            tile->is_occupied = true;
        }
    }
}

void Action_ToggleTowerPreview()
{
    // 3 way switch
    if (ui.placement.active)
    {
        if (ui.placement.tower_a_selected)
        {
            ui.placement.tower_a_selected = !ui.placement.tower_a_selected;
        }
        else
        {
            ui.placement.active = !ui.placement.active;
        }
    }
    else
    {
        ui.placement.active = true;
        ui.placement.tower_a_selected = true;
    }
}

void Action_ToggleCraftingPanel()
{
    PlayAudioFile(&Res.audio.click_hi, false, 90);

    ui.crafting.visible = !ui.crafting.visible;
    UiElement* panel = &uiElements.elements[ui.crafting.tower_panel];
    UiElement* items = &uiElements.elements[ui.crafting.items_panel];
    panel->visible = ui.crafting.visible;
    items->visible = ui.crafting.visible;

    // show/hide child elements
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
    PlayAudioFile(&Res.audio.click_lo, false, 90);
    UiElement* startBtn = &uiElements.elements[ui.menu.start_game_button];
    UiElement* mapsPanel = &uiElements.elements[ui.menu.map_selection_panel];
    UiElement* exitBtn = &uiElements.elements[ui.menu.exit_game_button];

    UiElement* towerSelection = &uiElements.elements[ui.placement.tower_selection_panel];
    UiElement* openCrafting = &uiElements.elements[ui.crafting.show_hide_button];
    UiElement* tmp2 = &uiElements.elements[ui.tmp_2];

    startBtn->visible = false;
    mapsPanel->visible = false;
    exitBtn->visible = false;

    towerSelection->visible = false;
    openCrafting->visible = true;
    tmp2->visible = true;

    navigation.in_menu = false;
    navigation.in_game = true;

    // TODO: trigger transition animation
    if (Res.audio.music.loaded)
    {
        // PlayAudioFile(&res.audio.music, true, 80);
    }
}

void Action_StartDrag()
{
    if (!ui.is_dragging)
    {
        ui.is_dragging = true;

        UiElement* el = ui.ui_focus_element;
        EntitySlotMap slotEntityMap = ui.crafting.item_slot_mapping[el->id];

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
    for (const auto& pair : ui.crafting.item_slot_mapping)
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
        EntitySlotMap mapping = ui.crafting.item_slot_mapping[dropTarget->id];

        // is default -> empty slot
        if (mapping.initial_slot_id == 0)
        {
            Position slotCenter = ItemSlotCenter(dropTarget->id);
            e->x = slotCenter.x;
            e->y = slotCenter.y;

            EntitySlotMap mapping = ui.crafting.item_slot_mapping[initialSlotId];
            ui.crafting.item_slot_mapping.erase(initialSlotId);

            // initial slot stays same, for reset
            mapping.current_slot_id = dropTarget->id;
            ui.crafting.item_slot_mapping[dropTarget->id] = mapping;
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

void Action_Exit()
{
    running = false;
}
