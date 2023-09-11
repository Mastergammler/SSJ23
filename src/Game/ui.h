#include "internal.h"

void InitializeUi(int uiElementCount, int layers);
void RecreateUiElements();

Position ItemSlotCenter(int slotId);
Position CalculateStartPixelPosition(Anchor anchor, Sprite sprite);
UniformGrid CalculateGridPositions(UiElement panel,
                                   float padding,
                                   float spacing,
                                   int items,
                                   Sprite sprite);

void UpdateMouseState();
void ProcessMouseActions();

void Action_ToggleTowerPreview();
void Action_ToggleCraftingPanel();
void Action_PlaceTower();
void Action_CraftTower();
void Action_SelectTowerType();
void Action_LoadUserSelectedMap();

void Action_TransitionToGame();
void Action_StartGame();
void Action_Exit();
void Action_GoToMenu();

void Action_StartDrag();
void Action_ResetItemToStartPosition();
void Action_DoDragging();
void Action_Drop();
