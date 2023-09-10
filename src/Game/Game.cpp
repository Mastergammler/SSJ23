#include "Loading/types.h"
#include "components.h"
#include "entities.h"
#include "internal.h"
#include "module.h"
#include "scenes.h"
#include "systems.h"
#include "ui.h"

Gameplay Game;

void StartGame()
{
    // TODO: better data structure solution for loading failed!
    if (!Res.bitmaps.ground.loaded || !Res.bitmaps.ui.loaded)
    {
        Log("Unable to rendere because resources not loaded");
        //  TODO: Play elevator music / show error screen
        this_thread::sleep_for(chrono::seconds(10));

        PostQuitMessage(0);
    }

    srand(time(0));
    InitializeEntities(Game.tile_map.rows * Game.tile_map.columns * 8);
    InitializeUi(64, 3);

    navigation.in_start_screen = true;

    // TODO: TMP
    Res.sprites.placeholder = Sprite{1, 1, 63, &Res.bitmaps.characters};
    Res.sprites.enemy_a = Sprite{1, 1, 24, &Res.bitmaps.characters};
    Res.sprites.broken_tower = Sprite{1, 1, 11, &Res.bitmaps.characters};

    // TODO: simple anim format -> just describe indices of the anim
    // -> the rest only once
    int offset = 8;
    Sprite* enemyWalkAnim = new Sprite[4]{Sprite{1,
                                                 1,
                                                 25 + offset,
                                                 &Res.bitmaps.characters},
                                          Sprite{1,
                                                 1,
                                                 26 + offset,
                                                 &Res.bitmaps.characters},
                                          Sprite{1,
                                                 1,
                                                 27 + offset,
                                                 &Res.bitmaps.characters},
                                          Sprite{1,
                                                 1,
                                                 28 + offset,
                                                 &Res.bitmaps.characters}};

    Res.animations.enemy_anim = SpriteAnimation{4, 0.1, enemyWalkAnim};

    Shader* shaders = new Shader[3]{Shader{COLOR_REPLACE, PALETTE_WHITE},
                                    Shader{COLOR_REPLACE, PALETTE_GRAY},
                                    Shader{COLOR_REPLACE, PALETTE_DARK_GRAY}};
    Keyframe* keyframes = new Keyframe[3]{Keyframe{0, 0.22},
                                          Keyframe{1, 0.08},
                                          Keyframe{2, 0.08}};

    Res.animations.enemy_hit = ShaderAnimation{3, keyframes, shaders};

#if DEBUG
    Action_GoToMenu();
    //   Action_StartGame();
#endif
}

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    // Static resources
    Game.tile_size = TileSize{.width = 16, .height = 16};
    InitStaticResources();

    Clock individualCounter = {};
    Log("Start Loading Resources");

    Time.Update();
    individualCounter.Update();
    Res.bitmaps = LoadSprites(hInstance, hdc);
    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    Res.audio = LoadAudioFiles();
    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();

    Game.tile_map = LoadTilemap(ABSOLUTE_RES_PATH + DEFAULT_MAP);
    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    LoadItems();
    Logf("  Items loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    Logf("Resources loaded in %.2f ms", Time.CheckDeltaTimeMs());

    // Setting size
    SetWindowSizeBasedOnTiles(Scale,
                              Game.tile_map.columns,
                              Game.tile_map.rows,
                              Game.tile_size.width,
                              Game.tile_size.height);

    InitScenes();
    StartGame();
}

/**
 * Called to update the information and buffer for the next frame
 */
// TODO: REFACTOR - with all the transition stuff this gets too messy
void UpdateFrame(ScreenBuffer& buffer)
{

    // special handling
    if (navigation.in_start_screen)
    {
        ShowLogoScreen(buffer);
        return;
    }

    if (!navigation.in_transition)
    {
        UpdateMouseState();
        ProcessMouseActions();
    }

    if (navigation.in_menu)
    {
        if (navigation.in_transition)
        {
            DrawToGameTransition(buffer);
        }
        else
        {
            FillWithTiles(buffer, Res.bitmaps.ui.tiles[56]);
        }
    }
    else if (navigation.in_game)
    {

        SpawningSystem();
        MoveEnemies();
        SimulateTower();
        MoveProjectiles();
        HandleProjectileCollisions();
        // Debug_PrintEnemyTilePositions();

        AnimateEntities();

        DrawGroundLayer(buffer);
#if DEBUG
        Debug_DrawTowerRangeAndDetection(buffer);
#endif
        DrawEntityLayer(buffer);

        // Debug_DrawEntityMovementPossibilities(buffer);
    }

    if (!navigation.in_transition)
    {
        DrawUiLayer(buffer);
    }
}
