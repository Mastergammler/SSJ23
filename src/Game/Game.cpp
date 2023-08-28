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

    navigation.in_start_screen = true;

    // TODO: TMP
    Res.sprites.placeholder = Sprite{1, 1, 63, &Res.bitmaps.characters};
    Res.sprites.tower_a = Sprite{1, 2, 0, &Res.bitmaps.characters};
    Res.sprites.tower_b = Sprite{1, 2, 2, &Res.bitmaps.characters};
    Res.sprites.enemy_a = Sprite{1, 1, 24, &Res.bitmaps.characters};

    // TODO: simple anim format -> just describe indices of the anim
    // -> the rest only once
    int offset = 8;
    Sprite* enemyWalkAnim = new Sprite[4]{
                                            Sprite{1,
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
    Game.tile_map = LoadMaps();
    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    individualCounter.Update();
    LoadItems();
    Logf("  Items loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());

    Logf("Resources loaded in %.2f ms", Time.CheckDeltaTimeMs());

    StartGame();
}

const float SHOW_LOGO_TIME = 2;
float timeInLogoScreen = 0;
int samplePosition = 0;
float timeSinceLastSample = 0;
float timePerSample = 0.15;

Animator anim = {};

void ShowLogoScreen(ScreenBuffer buffer)
{
    int centerX = scale.draw_width / 2 - Res.bitmaps.logo.width / 2;
    int centerY = scale.draw_height / 2 - Res.bitmaps.logo.height / 2;
    timeInLogoScreen += Time.delta_time_real;

    Shader shader = {};

    if (timeInLogoScreen > SHOW_LOGO_TIME)
    {
        // TODO: refactor - quite a ugly shader animator
        timeSinceLastSample += Time.delta_time_real;
        shader.type = COLOR_REPLACE;
        switch (samplePosition)
        {
            case 1:
            {
                shader.shader_color = PALETTE_GRAY;
            }
            break;
            case 2:
            {
                shader.shader_color = PALETTE_DARK_GRAY;
            }
            break;
            case 3:
            {
                shader.shader_color = PALETTE_DARK_BROWN;
            }
            break;
            case 4:
            {
                shader.shader_color = BG_COLOR;
            }
            break;
            case 5:
            case 6:
            case 7:
            case 8: break;
            case 9: Action_GoToMenu(); break;
            default: shader.type = NONE; break;
        }
        if (timeSinceLastSample > timePerSample)
        {
            timeSinceLastSample -= timePerSample;
            samplePosition++;
        }
    }
    DrawBitmap(buffer, Res.bitmaps.logo, centerX, centerY, shader);
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
    // special handling
    if (navigation.in_start_screen)
    {
        ShowLogoScreen(buffer);
        return;
    }

    UpdateMouseState();
    ProcessMouseActions();

    if (navigation.in_menu)
    {
        FillWithTiles(buffer, Res.bitmaps.ui.tiles[56]);
    }
    else if (navigation.in_game)
    {
        MoveEnemies();
        SimulateTower();
        MoveProjectiles();
        // Debug_PrintEnemyTilePositions();
        AnimateEntities();

        DrawGroundLayer(buffer);
        Debug_DrawTowerRangeAndDetection(buffer);
        DrawEntityLayer(buffer);

        // Debug_DrawEntityMovementPossibilities(buffer);
    }
    DrawUiLayer(buffer);
}
