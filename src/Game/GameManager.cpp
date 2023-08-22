#include "internal.h"
#include "module.h"

BitmapCache sprites = {};
SoundCache audio = {};
Tilemap tileMap = {0, 0, 0};
Tile tileSize = Tile{.width = 16, .height = 16};

void StartGame()
{
    // TODO: better data structure solution for loading failed!
    if (!sprites.ground.loaded || !sprites.ui.loaded)
    {
        Log("Unable to rendere because resources not loaded");
        //  TODO: Play elevator music / show error screen
        this_thread::sleep_for(chrono::seconds(10));

        PostQuitMessage(0);
    }

    InitializeUi(8, 3);

    actionState.crafting_panel_id = CreatePanel(2, 10, 8, 4, 0.5, false);

    int btn1 = CreateButton(3, 2, 0.5, [] {
        PlayAudioFile(&audio.click_hi, false, 90);
        Action_ToggleCraftingPanel();
    });

    int btn2 = CreateButton(5, 4, 0.5, [] {
        PlayAudioFile(&audio.click_lo, false, 90);
        Action_ToggleTowerPreview();
    });
    int btn3 = CreateButton(8, 4, 0.5, [] {
        PlayAudioFile(&audio.pop_hi, false, 90);
    });
    int btn4 = CreateButton(12, 2, 0.5, [] {
        PlayAudioFile(&audio.pop_lo, false, 90);
    });

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
    counter.Update();
    individualCounter.Update();

    sprites = LoadSprites(hInstance, hdc);

    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    audio = LoadAudioFiles();

    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    tileMap = LoadMaps();

    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    Logf("Resources loaded in %.2f ms", counter.CheckDeltaTimeMs());

    StartGame();
}

// NOTE: idea for fade animations
//  i could also just use the different colors
//  - from white to gray, to dark gray to dark brown to black screen
//  -> Could work similarly well
//  => I need a animation component!!!!

/**
 * Called to update the information and buffer for the next frame
 * TODO: is there any value in passing the buffer every time?
 */
void UpdateFrame(ScreenBuffer& buffer)
{
    UpdateMouseState();
    ProcessMouseActions();

    DrawGroundLayer(buffer);
    DrawEntityLayer(buffer);
    DrawUiLayer(buffer);
}
