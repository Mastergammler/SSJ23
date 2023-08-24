#include "internal.h"

void InitializeUi(int uiElementCount, int layers);
int CreateButton(int mapX,
                 int mapY,
                 float offset,
                 Action onClick,
                 bool visible);

int CreatePanel(int tileX,
                int tileY,
                int xSize,
                int ySize,
                float offset,
                bool visible);

void UpdateMouseState();
void ProcessMouseActions();

void Action_ToggleTowerPreview();
void Action_ToggleCraftingPanel();
void Action_PlaceTower();
void Action_SpawnEnemy();
