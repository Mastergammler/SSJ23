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
    ui.ui_focus = hovered->id != NullElement.id;
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
        if (ui.ui_focus)
        {
            Action_ResetItemToStartPosition();
        }
        else
        {
            Action_ToggleTowerPreview();
        }
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
    UiElement* craftBtn = &uiElements.elements[ui.crafting.crafting_button];
    panel->visible = ui.crafting.visible;
    items->visible = ui.crafting.visible;
    craftBtn->visible = ui.crafting.visible;

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
        // PlayAudioFile(&Res.audio.music, true, 80);
    }
}

EntitySlotMap* FindBySlotId(int slotId)
{
    for (int i = 0; i < ui.crafting.item_count; i++)
    {
        EntitySlotMap* cur = &ui.crafting.slot_map[i];
        if (cur->current_slot_id == slotId) return cur;
    }
    return NULL;
}

EntitySlotMap* FindByEntityId(int entityId)
{
    for (int i = 0; i < ui.crafting.item_count; i++)
    {
        EntitySlotMap* cur = &ui.crafting.slot_map[i];
        if (cur->entity_id == entityId) return cur;
    }
    return NULL;
}

void Action_StartDrag()
{
    if (!ui.is_dragging)
    {
        UiElement* el = ui.ui_focus_element;
        if (el->flags & DRAG_SOURCE)
        {
            EntitySlotMap* slotEntityMap = FindBySlotId(el->id);
            if (!slotEntityMap) return;

            ui.dragged_entity_id = slotEntityMap->entity_id;
            ui.is_dragging = true;
        }
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

void ResetSlotAndEntity(EntitySlotMap* slotMap)
{
    if (slotMap)
    {
        Entity* e = &entities.units[slotMap->entity_id];
        Position slotCenter = ItemSlotCenter(slotMap->initial_slot_id);
        e->x = slotCenter.x;
        e->y = slotCenter.y;

        slotMap->current_slot_id = slotMap->initial_slot_id;
    }
}

EntitySlotMap* FindNextAvailablePlacementSlot()
{
    for (int i = 0; i < ui.placement.item_count; i++)
    {
        EntitySlotMap* slotMap = &ui.placement.slot_map[i];

        // not all items are usable as slots
        if (slotMap->initial_slot_id != NullElement.id &&
            slotMap->entity_id == EntityZero.id)
        {
            return slotMap;
        }
    }

    return NULL;
}

void Action_CraftTower()
{
    EntitySlotMap* bulletSlot = FindBySlotId(ui.crafting.tower_slot_bullet);
    EntitySlotMap* pillarSlot = FindBySlotId(ui.crafting.tower_slot_pillar);

    if (bulletSlot && pillarSlot)
    {
        EntitySlotMap* availableSlot = FindNextAvailablePlacementSlot();
        // TODO: order right?
        ResetSlotAndEntity(bulletSlot);
        ResetSlotAndEntity(pillarSlot);

        if (availableSlot)
        {

            Position slotCenter = ItemSlotCenter(availableSlot->current_slot_id);
            int entityId = CreatCannonTypeEntity(slotCenter.x,
                                                 slotCenter.y,
                                                 bulletSlot->entity_id,
                                                 pillarSlot->entity_id);
            availableSlot->entity_id = entityId;
            PlayAudioFile(&Res.audio.craft_success, false, 100);
            return;
        }
    }

    PlayAudioFile(&Res.audio.craft_error, false, 100);
}

void Action_ResetItemToStartPosition()
{
    UiElement* hoveredElement = ui.ui_focus_element;

    // We're only resetting on drop targets
    // On DRAG_SOURCE it should not matter, as the position would be the same
    if (hoveredElement->flags & DROP_TARGET)
    {
        EntitySlotMap* map = FindBySlotId(hoveredElement->id);
        ResetSlotAndEntity(map);
    }
}

void Action_Drop()
{
    Entity* e = &entities.units[ui.dragged_entity_id];
    UiElement* hoveredElement = ui.ui_focus_element;

    EntitySlotMap* entitySlot = FindByEntityId(e->id);
    assert(entitySlot);

    if (hoveredElement->flags & DROP_TARGET)
    {
        EntitySlotMap* targetSlotMapping = FindBySlotId(hoveredElement->id);
        if (targetSlotMapping)
        {
            Action_ResetItemToStartPosition();
        }

        Position slotCenter = ItemSlotCenter(hoveredElement->id);
        e->x = slotCenter.x;
        e->y = slotCenter.y;

        entitySlot->current_slot_id = hoveredElement->id;
    }
    else
    {
        EntitySlotMap* draggedEntity = FindByEntityId(ui.dragged_entity_id);
        ResetSlotAndEntity(draggedEntity);
    }

    ui.dragged_entity_id = EntityZero.id;
    ui.is_dragging = false;
}

void Action_Exit()
{
    running = false;
}
