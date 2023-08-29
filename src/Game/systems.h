#pragma once

#include "internal.h"

//----------------
//  Movement
void MoveEnemies();
void MoveProjectiles();

//----------------
//  Collisions
UiElement* FindHighestLayerCollision(int x, int y);
void HandleProjectileCollisions();

//---------------
//  Simulation
void SimulateTower();
void SpawningSystem();

//---------------
//  Animations
void AnimateEntities();

//----------------
//  Rendering
void RenderEntities(ScreenBuffer buffer);
void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet);
void RenderEntitiesOfType(ScreenBuffer buffer, EntityType type);

//----------------
//  Debug
void Debug_DrawTowerRangeAndDetection(ScreenBuffer buffer);
void Debug_DrawEntityMovementPossibilities(ScreenBuffer buffer);
void Debug_PrintEnemyTilePositions();
