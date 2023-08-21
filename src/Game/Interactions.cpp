#include "internal.h"

InteractionState actionState;

void Action_ToggleTowerPreview()
{
    actionState.show_tower_placement = !actionState.show_tower_placement;
}

void Action_ToggleCraftingPanel()
{
    actionState.show_crafting_panel = !actionState.show_crafting_panel;
}
