#pragma once

#include "Components/types.h"
#include "Entities/types.h"
#include "Loading/types.h"
#include "UI/types.h"
#include "module.h"

// Resource cache
// NOTE: i kinda like this concept
// maybe i can do more of those
// - Mem -> for ECS stuff
// - Game -> for tile maps etc (ui also)
// Has also the benefit, that you can text search for all it's usages
// And maybe even free etc it all together
// Maybe stuff should be grouped in a way, where it wants to be
// allocated and freed together?
extern Resources Res;

// Definitions
extern map<TileEnv, int> pathMap;
extern map<TileEnv, int> grassMap;
extern TileSize _tileSize;
extern TileMap _tileMap;

// State tracking
extern UiState ui;
extern MouseState mouseState;
extern Navigation navigation;

// Stores
extern UiElementStorage uiElements;
extern EntityStore entities;
extern TowerStore towers;
extern EnemyStore enemies;
extern ComponentStore components;

// Zeros
const Entity EntityZero = Entity{.initialized = false,
                                 .id = -1,
                                 .storage_id = -1};

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
TileMap LoadMaps();
SpriteSheet ConvertFromSheet(BitmapBuffer sheet, int tileWidth, int tileHeight);

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);

int CreateTowerEntity(int x, int y, Sprite sprite);
int CreateItemEntity(int x, int y, Sprite sprite);
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation);
void InitializeEntities(int storeCount);
