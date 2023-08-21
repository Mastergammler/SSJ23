#pragma once

#include "module.h"
#include "types.h"

extern map<TileEnv, int> pathMap;
extern MouseState mouseState;
extern Tile tileSize;
extern UiElementStorage storage;
extern InteractionState actionState;
extern BitmapCache sprites;
extern SoundCache audio;
extern Tilemap tileMap;
extern Tile tileSize;

void UpdateMouseState();

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
Tilemap LoadMaps();

void InitializeUi(int elementCount);
int CreateButton(Tilemap& map,
                 int mapX,
                 int mapY,
                 float offset,
                 Action onClick);
void RenderButtons(ScreenBuffer& buffer, SpriteSheet& sheet);
UiElement* FindHighestLayerCollision(int x, int y);

void ProcessMouseActions();
void Action_ToggleTowerPreview();
void Action_ToggleCraftingPanel();

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);
