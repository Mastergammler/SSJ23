#include "internal.h"
#include "module.h"
#include "systems.h"
#include "ui.h"

BitmapCache _bitmaps;
SoundCache _audio;
SpriteCache _sprites;
AnimationCache _animations;
TileMap _tileMap;
TileSize _tileSize = TileSize{.width = 16, .height = 16};

void StartGame()
{
    navigation.in_menu = true;

    // TODO: better data structure solution for loading failed!
    if (!_bitmaps.ground.loaded || !_bitmaps.ui.loaded)
    {
        Log("Unable to rendere because resources not loaded");
        //  TODO: Play elevator music / show error screen
        this_thread::sleep_for(chrono::seconds(10));

        PostQuitMessage(0);
    }

    InitializeUi(8, 3);
    InitializeEntities(_tileMap.rows * _tileMap.columns * 8);

    // TODO: TMP
    _sprites.tower_a = Sprite{1, 2, 0, &_bitmaps.characters};
    _sprites.tower_b = Sprite{1, 2, 2, &_bitmaps.characters};
    _sprites.enemy_a = Sprite{1, 1, 24, &_bitmaps.characters};

    Sprite* enemyWalkAnim = new Sprite[4]{
        Sprite{1, 1, 25, &_bitmaps.characters},
        Sprite{1, 1, 26, &_bitmaps.characters},
        Sprite{1, 1, 27, &_bitmaps.characters},
        Sprite{1, 1, 28, &_bitmaps.characters},
    };

    _animations.enemy_anim = SpriteAnimation{4, 0.1, enemyWalkAnim};

    // TODO: remove
    // Action_StartGame();
}

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    FpsCounter individualCounter = {};
    Log("Start Loading Resources");

    measure.Update();
    individualCounter.Update();
    _bitmaps = LoadSprites(hInstance, hdc);
    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    _audio = LoadAudioFiles();
    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    _tileMap = LoadMaps();

    for (int i = 0; i < _tileMap.spawn_count; i++)
    {
        Tile tar = *_tileMap.spawns[i];
        Logf("Spawn %d: %d,%d", i, tar.x, tar.y);
    }

    for (int i = 0; i < _tileMap.target_count; i++)
    {
        Tile tar = *_tileMap.targets[i];
        Logf("Target %d: %d,%d", i, tar.x, tar.y);
    }

    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    Logf("Resources loaded in %.2f ms", measure.CheckDeltaTimeMs());

    StartGame();
}

// NOTE: idea for fade animations
//  i could also just use the different colors
//  - from white to gray, to dark gray to dark brown to black screen
//  -> Could work similarly well
//  => I need a animation component!!!!

/**
 * Called to update the information and buffer for the next frame
 */
void UpdateFrame(ScreenBuffer& buffer)
{

    UpdateMouseState();
    ProcessMouseActions();

    if (navigation.in_menu)
    {
        FillWithTiles(buffer, _bitmaps.ui.tiles[56]);
    }
    else if (navigation.in_game)
    {

        MoveEnemies();
        AnimateEntities();

        DrawGroundLayer(buffer);
        DrawEntityLayer(buffer);
        // Debug_DrawEntityMovementPossibilities(buffer);
    }
    DrawUiLayer(buffer);
}
