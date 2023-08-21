#pragma once

#include "module.h"
#include "types.h"

extern map<TileState, int> PathMap;
extern MouseState mouseState;
extern Tile TileSize;
extern UiElementStorage storage;

void UpdateMouseState();

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
Tilemap LoadMaps();

void DrawTilemap(Tilemap& map, ScreenBuffer& buffer, SpriteSheet& sheet);
void InitializeUi(int elementCount);
int CreateButton(Tilemap& map, int mapX, int mapY, float offset);
void RenderButtons(ScreenBuffer& buffer, SpriteSheet& sheet);
