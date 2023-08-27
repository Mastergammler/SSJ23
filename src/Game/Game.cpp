#include "internal.h"
#include "module.h"
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

    InitializeEntities(Game.tile_map.rows * Game.tile_map.columns * 8);
    InitializeUi(64, 3);

    navigation.in_menu = true;
    UiElement* startBtn = &uiElements.elements[ui.menu.start_game_button];
    UiElement* mapsPanel = &uiElements.elements[ui.menu.map_selection_panel];
    UiElement* exitBtn = &uiElements.elements[ui.menu.exit_game_button];

    startBtn->visible = true;
    mapsPanel->visible = true;
    exitBtn->visible = true;

    // TODO: TMP
    Res.sprites.tower_a = Sprite{1, 2, 0, &Res.bitmaps.characters};
    Res.sprites.tower_b = Sprite{1, 2, 2, &Res.bitmaps.characters};
    Res.sprites.enemy_a = Sprite{1, 1, 24, &Res.bitmaps.characters};

    Sprite* enemyWalkAnim = new Sprite[4]{
                                            Sprite{1,
                                                   1,
                                                   25,
                                                   &Res.bitmaps.characters},
                                            Sprite{1,
                                                   1,
                                                   26,
                                                   &Res.bitmaps.characters},
                                            Sprite{1,
                                                   1,
                                                   27,
                                                   &Res.bitmaps.characters},
                                            Sprite{1,
                                                   1,
                                                   28,
                                                   &Res.bitmaps.characters},
    };

    Res.animations.enemy_anim = SpriteAnimation{4, 0.1, enemyWalkAnim};

    // TODO: remove
    Action_StartGame();
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

    FpsCounter individualCounter = {};
    Log("Start Loading Resources");

    measure.Update();
    individualCounter.Update();
    Res.bitmaps = LoadSprites(hInstance, hdc);
    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    Res.audio = LoadAudioFiles();
    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    Game.tile_map = LoadMaps();
    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    LoadItems();
    Logf("  Items loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

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
        FillWithTiles(buffer, Res.bitmaps.ui.tiles[56]);
    }
    else if (navigation.in_game)
    {

        MoveEnemies();
        // Debug_PrintEnemyTilePositions();
        AnimateEntities();

        DrawGroundLayer(buffer);
        DrawEntityLayer(buffer);

        // Debug_DrawEntityMovementPossibilities(buffer);
    }
    DrawUiLayer(buffer);
}
