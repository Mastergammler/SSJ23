#include "internal.h"
#include "module.h"

BitmapCache bitmaps;
SoundCache audio;
SpriteCache sprites;
TileMap tileMap;
TileSize tileSize = TileSize{.width = 16, .height = 16};

void StartGame()
{
    // TODO: better data structure solution for loading failed!
    if (!bitmaps.ground.loaded || !bitmaps.ui.loaded)
    {
        Log("Unable to rendere because resources not loaded");
        //  TODO: Play elevator music / show error screen
        this_thread::sleep_for(chrono::seconds(10));

        PostQuitMessage(0);
    }

    InitializeUi(8, 3);
    InitializeEntities(tileMap.rows * tileMap.columns);

    // TODO: TMP
    sprites.tower_a = Sprite{1, 2, 0, &bitmaps.characters};
    sprites.tower_b = Sprite{1, 2, 1, &bitmaps.characters};
    sprites.enemy_a = Sprite{1, 1, 24, &bitmaps.characters};

    CreateEnemyEntity(100, 200, sprites.enemy_a, WEST);
    CreateEnemyEntity(132, 200, sprites.enemy_a, SOUTH);
    CreateEnemyEntity(100, 150, sprites.enemy_a, WEST);

    // if (Audio.music.loaded) { PlayAudioFile(&Audio.music, true, 80); }
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
    bitmaps = LoadSprites(hInstance, hdc);
    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    audio = LoadAudioFiles();
    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    tileMap = LoadMaps();
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

    MoveEnemies();

    DrawGroundLayer(buffer);
    DrawEntityLayer(buffer);
    DrawUiLayer(buffer);
}
