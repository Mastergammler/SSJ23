#include "../Audio/module.h"
#include "../IO/module.h"
#include "../Rendering/module.h"
#include "internal.h"
#include "module.h"

struct BitmapCache
{
    BitmapBuffer cursor_sprite = {};
    BitmapBuffer tile_highlight = {};

    SpriteSheet ground = {};
    SpriteSheet ui = {};
};

struct SoundCache
{
    WaveBuffer click_hi;
    WaveBuffer click_lo;
    WaveBuffer pop_lo;
    WaveBuffer pop_hi;

    WaveBuffer music;
};

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";
global_var BitmapCache Bitmaps = {};
global_var SoundCache Audio = {};
global_var Tilemap TitleMap = {0, 0, 0};
global_var Tile TileSize = Tile{.width = 16, .height = 16};

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

    if (Audio.music.loaded) { PlayAudioFile(&Audio.music, true, 80); }
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

    BitmapBuffer bmp1 =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Cursor.bmp", hInstance, hdc);
    if (bmp1.loaded) { Bitmaps.cursor_sprite = bmp1; }
    BitmapBuffer bmp2 = LoadSprite(ABSOLUTE_RES_PATH + "Test/TileHighlight.bmp",
                                   hInstance,
                                   hdc);
    if (bmp2.loaded) { Bitmaps.tile_highlight = bmp2; }

    BitmapBuffer tilesSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Tiles_8x8.bmp", hInstance, hdc);
    if (tilesSheet.loaded)
    {
        Bitmaps.ground =
            ConvertFromSheet(tilesSheet, TileSize.width, TileSize.height);
    }

    BitmapBuffer uiSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/UI_8x8.bmp", hInstance, hdc);
    if (uiSheet.loaded)
    {
        Bitmaps.ui = ConvertFromSheet(uiSheet, TileSize.width, TileSize.height);
    }

    Logf("  Sprites loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    WaveBuffer wave = LoadWaveFile(ABSOLUTE_RES_PATH + "Music/TitleTheme.wav");
    if (wave.loaded) { Audio.music = wave; }

    // Sounds
    WaveBuffer clickLo =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Lo.wav");
    if (clickLo.loaded) { Audio.click_lo = clickLo; }
    WaveBuffer clickHi =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Hi.wav");
    if (clickHi.loaded) { Audio.click_hi = clickHi; }
    WaveBuffer popLo = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Lo.wav");
    if (popLo.loaded) { Audio.pop_lo = popLo; }
    WaveBuffer popHi = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Hi.wav");
    if (popHi.loaded) { Audio.pop_hi = popHi; }

    Logf("  Audio loaded in %.2f ms", individualCounter.CheckDeltaTimeMs());
    individualCounter.Update();

    TitleMap = LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");

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

/**
 * Called to update the information and buffer for the next frame
 */
void UpdateScreen(ScreenBuffer& buffer)
{
    UpdateMouseState();

    // TODO: refactor this
    //  1.
    //   - what is the context im in (menu / game)
    //   - mouse collisions in order
    //  2. Then draw accordingly
    //   - layer by layer etc
    //   - according to states

    int* tile = TitleMap.idMap;
    for (int y = 0; y < TitleMap.rows; y++)
    {
        for (int x = 0; x < TitleMap.columns; x++)
        {
            int tileId = *tile++;
            int tileIdx = y * TitleMap.columns + x;
            // TODO: use tile size
            int tileX = x * TileSize.width;
            int tileY = y * TileSize.height;
            if (tileId == 0)
            {
                DrawBitmap(buffer, Bitmaps.ground.tiles[9], tileX, tileY);
            }
            else if (tileId == 1)
            {
                ContextTile tile = TitleMap.context_tiles[tileIdx];

                TileState ts;

                // is special case - for inner corners
                if (tile.adjacent > 0b11110000)
                {
                    ts = static_cast<TileState>(tile.adjacent);
                }
                else
                {
                    // only compare the first 4 bits
                    ts = static_cast<TileState>(tile.adjacent & 0b11110000);
                }

                int sheetIdx = PATH_MAP[ts];
                DrawBitmap(buffer,
                           Bitmaps.ground.tiles[sheetIdx],
                           tileX,
                           tileY);
            }
        }
    }

    // TODO: UI layer
    //  - block mouse over below it?
    //  -

    // draw button
    // TODO: state management for mouse and objects before the draws

    int offset = TileSize.width / 2;
    int buttonStartX = TileSize.width * (TitleMap.columns - 3) + offset;
    int buttonStartY = TileSize.height * (TitleMap.rows - 2) + offset;
    int buttonEndX = buttonStartX + 2 * TileSize.width;
    int buttonEndY = buttonStartY + TileSize.height;

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

    bool mouseOverButton;
    int sheetStartIdx;
    if (mouseState.x > buttonStartX && mouseState.x < buttonEndX &&
        mouseState.y > buttonStartY && mouseState.y < buttonEndY)
    {
        sheetStartIdx = 10;
        mouseOverButton = true;
    }
    else
    {
        sheetStartIdx = 8;
        mouseOverButton = false;
    }

    // draw overlay section stuff
    if (mouseState.left_down && !mouseOverButton)
    {
        // TODO: tile size information system
        int tileIdxX = mouseState.x / TileSize.width;
        int tileIdxY = mouseState.y / TileSize.height;

        int tileXStart = tileIdxX * TileSize.width;
        int tileYStart = tileIdxY * TileSize.height;

        int tileId = TitleMap.GetTileId(mouseState.x, mouseState.y);

        // mockup for blocking placements on the way
        if (tileId == 0)
        {
            DrawBitmap(buffer, Bitmaps.ui.tiles[0], tileXStart, tileYStart);
        }
        else if (tileId == 1)
        {
            DrawBitmap(buffer, Bitmaps.ui.tiles[2], tileXStart, tileYStart);
        }
    }

    DrawTiles(buffer,
              buttonStartX,
              buttonStartY,
              Bitmaps.ui,
              sheetStartIdx,
              2,
              1);

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

    // this shows again, that the button state must be disconnected from the
    // drawings
    if (drawPanel)
    {
        DrawPanel(buffer, (8 + 2 * 16), (8 + 4 * 16), Bitmaps.ui, 16, 8, 4);
    }

    // draw mouse
    DrawBitmap(buffer, Bitmaps.cursor_sprite, mouseState.x, mouseState.y, true);
}
