#pragma once

#include "entity.h"
#include "module.h"

// Resource cache
extern BitmapCache _bitmaps;
extern SoundCache _audio;
extern SpriteCache _sprites;

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

void UpdateMouseState();

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
TileMap LoadMaps();

void InitializeUi(int elementCount, int layers);
void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet);
UiElement* FindHighestLayerCollision(int x, int y);

void ProcessMouseActions();
void Action_ToggleTowerPreview();
void Action_ToggleCraftingPanel();
void Action_PlaceTower();

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);

int CreateTowerEntity(int x, int y, Sprite sprite);
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed);
void MoveEnemies();

void InitializeEntities(int storeCount);
void RenderEntities(ScreenBuffer buffer);
void Debug_DrawEntityMovementPossibilities(ScreenBuffer buffer);
