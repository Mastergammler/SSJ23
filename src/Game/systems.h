#pragma once

#include "internal.h"

//----------------
//  Movement
void MoveEnemies();

//----------------
//  Collisions
UiElement* FindHighestLayerCollision(int x, int y);

//---------------
//  Animations
void AnimateEntities();

//----------------
//  Rendering
void RenderEntities(ScreenBuffer buffer);
void RenderUiElements(ScreenBuffer& buffer, SpriteSheet& sheet);
void RenderEntitiesOnTop(ScreenBuffer buffer, EntityType type);

//----------------
//  Debug
void Debug_DrawEntityMovementPossibilities(ScreenBuffer buffer);
