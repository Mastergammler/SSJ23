#pragma once

#include "Components/types.h"
#include "Entities/types.h"
#include "Loading/types.h"
#include "Rendering/colors.h"
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
extern ProjectileStore projectiles;

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
TileMap LoadTilemap(string mapPath);
SpriteSheet ConvertFromSheet(BitmapBuffer sheetBitmap, TileSize tileSize);

void DrawGroundLayer(ScreenBuffer buffer);
void DrawEntityLayer(ScreenBuffer buffer);
void DrawUiLayer(ScreenBuffer buffer);

// UTILS
Position TileToDrawPosition(Tile tile);
Position TileCenterPosition(Tile tile);
