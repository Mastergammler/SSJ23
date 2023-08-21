#include "internal.h"
#include "module.h"

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var BitmapCache Bitmaps = {};
global_var SoundCache Audio = {};
global_var Tilemap Map = {0, 0, 0};
Tile TileSize = Tile{.width = 16, .height = 16};

void StartGame()
{
    // TODO: better data structure solution for loading failed!
    if (!Bitmaps.ground.loaded || !Bitmaps.ui.loaded)
    {
        Log("Unable to rendere because resources not loaded");
        //  TODO: Play elevator music / show error screen
        this_thread::sleep_for(chrono::seconds(10));

        PostQuitMessage(0);
    }

    InitializeUi(8);
    int btn1 = CreateButton(Map, 3, 2, 0.5);
    int btn2 = CreateButton(Map, 5, 4, 0.5);
    int btn3 = CreateButton(Map, 8, 4, 0.5);
    int btn4 = CreateButton(Map, 12, 2, 0.5);
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

    Bitmaps = LoadSprites(hInstance, hdc);

    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    Audio = LoadAudioFiles();

    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    Map = LoadMaps();

    Logf("  Tilemap loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    Logf("Resources loaded in %.2f ms", counter.CheckDeltaTimeMs());

    StartGame();
}

// NOTE: idea for fade animations
//  i could also just use the different colors
//  - from white to gray, to dark gray to dark brown to black screen
//  -> Could work similarly well
//  => I need a animation component!!!!

// FIXME: this needs to be save somewhere
bool drawPanel = false;
bool showTower = false;

/**
 * Called to update the information and buffer for the next frame
 * TODO: is there any value in passing the buffer every time?
 */
void UpdateScreen(ScreenBuffer& buffer)
{
    UpdateMouseState();

    // Lowest layer
    DrawTilemap(Map, buffer, Bitmaps.ground);

    // draw button
    // Do i want to have a button struct that holds the position info?
    // Probably right? And also how to draw it?
    // Because else it's going to be hard to check against it's position

    // NOTE: with this i can see i'm now getting into the real entity design
    // because now i need a system to check, what is below me
    // and i need to check this for all my Entities
    // - for tiles
    // - for enemies
    // - for ui
    // => And all in the right order
    // >>>> This will be the design challenge for next week!!! <<<<<<<

    // this is an entity basically?
    // something that needs to exist already?
    // something that needs to be persisted somewhere

    // TEST: steps to do
    //  - when i have all entities that are interactable
    //  - go over each of them and check for collisions
    //  - for the first found set that state

    // FIXME: this needs to live somewhere else as well
    // this is the to action map
    int btn1 = 0;

    // THIS IS COLLISION CHECK BASICALLY
    bool mouseOverButton;
    if (mouseState.x > storage.elements[btn1].x_start &&
        mouseState.x < storage.elements[btn1].x_end &&
        mouseState.y > storage.elements[btn1].y_start &&
        mouseState.y < storage.elements[btn1].y_end)
    {
        mouseOverButton = true;
    }
    else { mouseOverButton = false; }

    // MAP INTERACTION LOGIC

    if (mouseState.right_clicked && !mouseOverButton)
    {
        showTower = !showTower;
    }

    if (mouseState.left_clicked)
    {
        if (mouseOverButton) { drawPanel = !drawPanel; }

        if (drawPanel)
            PlayAudioFile(&Audio.click_hi, false, 90);
        else
            PlayAudioFile(&Audio.pop_lo, false, 90);
    }

    if (mouseState.right_clicked)
    {
        if (mouseOverButton)
            PlayAudioFile(&Audio.click_lo, false, 90);
        else
            PlayAudioFile(&Audio.pop_hi, false, 90);
    }

    if (showTower && !mouseOverButton)
    {
        // TODO: tile size information system
        int tileIdxX = mouseState.x / TileSize.width;
        int tileIdxY = mouseState.y / TileSize.height;

        int tileXStart = tileIdxX * TileSize.width;
        int tileYStart = tileIdxY * TileSize.height;

        int tileId = Map.GetTileId(mouseState.x, mouseState.y);

        int uiIdx;
        int shadowIdx;
        int towerIdx;

        // tile based impact
        if (tileId == 0)
        {
            uiIdx = 0;
            shadowIdx = 17;
            towerIdx = 1;
        }
        else if (tileId == 1)
        {
            uiIdx = 2;
            shadowIdx = 16;
            towerIdx = 0;
        }

        // TODO: Layering setup etc
        // Entity layer has to be drawn, top to bottem (from tile view)
        DrawBitmap(buffer,
                   Bitmaps.characters.tiles[shadowIdx],
                   tileXStart,
                   tileYStart);
        DrawBitmap(buffer, Bitmaps.ui.tiles[uiIdx], tileXStart, tileYStart);
        DrawTiles(buffer,
                  tileXStart,
                  tileYStart + TileSize.height / 2,
                  Bitmaps.characters,
                  towerIdx,
                  1,
                  2);
    }

    // state based impact
    // int sheetStartIdx = mouseOverButton ?
    // storage.elements[btn1].hover_sheet_start_idx
    //                                    :
    //                                    storage.elements[btn1].default_sheet_start_idx;

    RenderButtons(buffer, Bitmaps.ui);
    //   DrawTiles(buffer,
    //             storage.elements[btn1].x_start,
    //             storage.elements[btn1].y_start,
    //             Bitmaps.ui,
    //             storage.elements[btn1].sprite_index,
    //             storage.elements[btn1].x_tiles,
    //             storage.elements[btn1].y_tiles);

    // this shows again, that the button state must be disconnected from the
    // drawings
    if (drawPanel)
    {
        DrawPanel(buffer, (8 + 2 * 16), (8 + 4 * 16), Bitmaps.ui, 16, 8, 4);
    }

    // draw mouse
    DrawBitmap(buffer, Bitmaps.cursor_sprite, mouseState.x, mouseState.y, true);
}
