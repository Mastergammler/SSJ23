#pragma once

#include "Components/types.h"
#include "Entities/types.h"
#include "Loading/types.h"
#include "UI/types.h"
#include "module.h"

struct Gameplay
{
    TileMap tile_map;
    TileSize tile_size;

    map<TileEnv, int> path_mappings;
    map<TileEnv, int> grass_mappings;
};

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
extern Gameplay Game;

// State tracking
extern UiState ui;
extern MouseState mouseState;
extern Navigation navigation;

// Stores
extern UiElementStorage uiElements;
extern ComponentStore components;
extern EntityStore entities;
extern TowerStore towers;
extern EnemyStore enemies;
extern ItemStore items;
extern CannonTypeStore cannons;

// Zeros
const Entity EntityZero = Entity{.initialized = false,
                                 .id = -1,
                                 .storage_id = -1};

const UiElement NullElement = {.visible = false,
                               .initialized = false,
                               .id = -1,
                               .layer = -1,
                               .on_click = [] {
                                   Log("Error: Null element on-click invoked!");
                               }};

void InitStaticResources();
BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc);
SoundCache LoadAudioFiles();
void LoadItems();
TileMap LoadMaps();
SpriteSheet ConvertFromSheet(BitmapBuffer sheet, int tileWidth, int tileHeight);

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);

int CreatCannonTypeEntity(int x, int y, int bulletItemId, int postItemId);
int CreateTowerEntity(int x, int y, Sprite bullet, Sprite pillar);
int CreateItemEntity(int x, int y, ItemData data);
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation);
void InitializeEntities(int storeCount);
