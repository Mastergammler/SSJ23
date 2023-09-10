#include "internal.h"

Entity* InitNextEntity();
int CreateProjectileEntity();
int CreatCannonTypeEntity(int x, int y, int bulletItemId, int postItemId);
int CreateTowerEntity(int x,
                      int y,
                      int cannonTypeId,
                      Sprite bulletSprite,
                      Sprite pillarSprite,
                      Sprite brokenSprite);
int CreateItemEntity(int x, int y, ItemData data);
int CreateEnemyEntity(int x,
                      int y,
                      Sprite sprite,
                      Direction direction,
                      int speed,
                      SpriteAnimation animation,
                      ShaderAnimation shaderAnimation);
void InitializeEntities(int storeCount);
