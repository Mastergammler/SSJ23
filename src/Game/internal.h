#pragma once

#include "Components/types.h"
#include "Entities/types.h"
#include "module.h"
#include "types.h"

// Resource cache
extern BitmapCache _bitmaps;
extern SoundCache _audio;
extern SpriteCache _sprites;
extern AnimationCache _animations;

// Definitions
extern map<TileEnv, int> pathMap;
extern map<TileEnv, int> grassMap;
extern TileSize _tileSize;
extern TileMap _tileMap;

// State tracking
extern UiState ui;
extern MouseState mouseState;

// Stores
extern UiElementStorage uiElements;
extern EntityStore entities;
extern TowerStore towers;
extern EnemyStore enemies;
extern ComponentStore components;

void UpdateMouseState();

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
TileMap LoadMaps();

void InitializeUi(int elementCount, int layers);

SpriteSheet ConvertFromSheet(BitmapBuffer sheet, int tileWidth, int tileHeight);
void ProcessMouseActions();
void Action_ToggleTowerPreview();
void Action_ToggleCraftingPanel();
void Action_PlaceTower();
void Action_SpawnEnemy();

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);

int CreateTowerEntity(int x, int y, Sprite sprite);
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation);
void InitializeEntities(int storeCount);
