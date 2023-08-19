#include "../Audio/module.h"
#include "../IO/module.h"
#include "../Rendering/module.h"
#include "../Win32/module.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"
#include "Tiling.cpp"

struct BitmapCache
{
    BitmapBuffer cursor_sprite = {};
    BitmapBuffer tile_highlight = {};

    int tile_count = 0;
    bool tiles_loaded = false;
    bool ui_loaded = false;
    BitmapBuffer* sheet = {};
    BitmapBuffer* uiTiles = {};

    void SetSheet(BitmapBuffer* sheet)
    {
        tiles_loaded = true;
        this->sheet = sheet;
    }

    void SetUiSheet(BitmapBuffer* sheet)
    {
        ui_loaded = true;
        uiTiles = sheet;
    }
};

struct SoundCache
{
    WaveBuffer click_hi;
    WaveBuffer click_lo;
    WaveBuffer pop_lo;
    WaveBuffer pop_hi;

    WaveBuffer music;
};

// have 2 maps, one per tileId
// and this map has as key the adjacent thing

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";
global_var BitmapCache bitmaps = {};
global_var SoundCache audio = {};

global_var const int TILE_ROWS = 12;
global_var const int TILE_COLUMNS = 15;
global_var Tilemap tileMap = {0, 0, 0};

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    // TODO: log loading performance
    Log(logger, "Start loading resources ...");

    BitmapBuffer bmp1 =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Cursor.bmp", hInstance, hdc);
    if (bmp1.loaded) { bitmaps.cursor_sprite = bmp1; }
    BitmapBuffer bmp2 = LoadSprite(ABSOLUTE_RES_PATH + "Test/TileHighlight.bmp",
                                   hInstance,
                                   hdc);
    if (bmp2.loaded) { bitmaps.tile_highlight = bmp2; }

    BitmapBuffer tilesSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Tiles_8x8.bmp", hInstance, hdc);
    if (tilesSheet.loaded)
    {
        // TODO: tile size info
        BitmapBuffer* tiles = ConvertFromSheet(tilesSheet, 16, 16);
        bitmaps.SetSheet(tiles);
    }

    BitmapBuffer uiSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/UI_8x8.bmp", hInstance, hdc);
    if (uiSheet.loaded)
    {
        // TODO: tile size!!!!
        BitmapBuffer* uiTiles = ConvertFromSheet(uiSheet, 16, 16);
        bitmaps.SetUiSheet(uiTiles);
    }

    // Not working - maybe i just create the cursor icon myself -> then i have
    // control over it! cursor = LoadCursorIcon(hInstance, ABSOLUTE_RES_PATH +
    // "Test/CursorTest.bmp");

    Log(logger, "Game Resources loaded");
    Log(logger, "Loading audio");

    WaveBuffer wave = LoadWaveFile(ABSOLUTE_RES_PATH + "Music/TitleTheme.wav");
    if (wave.loaded) { audio.music = wave; }

    // Sounds
    WaveBuffer clickLo =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Lo.wav");
    if (clickLo.loaded) { audio.click_lo = clickLo; }
    WaveBuffer clickHi =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Hi.wav");
    if (clickHi.loaded) { audio.click_hi = clickHi; }
    WaveBuffer popLo = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Lo.wav");
    if (popLo.loaded) { audio.pop_lo = popLo; }
    WaveBuffer popHi = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Hi.wav");
    if (popHi.loaded) { audio.pop_hi = popHi; }

    Log(logger, "Audio files Loaded");
    Log(logger, "Loading Tilemap");

    tileMap = LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");

    Log(logger, "Tilemap loaded");

    // FIXME: this is independant from the map init ....
    // just make it load by file
    int tilemapInit[TILE_ROWS][TILE_COLUMNS] = {
        {0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {1, 1, 1, 1, 0, 0, 1, 1, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    };
    // map.AssignMap(&tilemapInit[0][0]);

    // memcpy(map.idMap, tilemapInit, sizeof(tilemapInit));
    // tilemapInit[y][x];

    // Start up all the things
    if (wave.loaded) { PlayAudioFile(&audio.music, true, 80); }
}

// FIXME: this should not be here
bool mouseRightLastState = false;
bool mouseLeftLastState = false;

// NOTE: idea for fade animations
//  i could also just use the different colors
//  - from white to gray, to dark gray to dark brown to black screen
//  -> Could work similarly well
//  => I need a animation component!!!!

/**
 * Called to update the information and buffer for the next frame
 */
void UpdateScreen(ScreenBuffer& buffer)
{
    // TODO: refactor this
    //  1.
    //   - mouse state etc
    //   - what is the context im in (menu / game)
    //   - mouse collisions in order
    //  2. Then draw accordingly
    //   - layer by layer etc
    //   - according to states

    // TODO: save tile sive somewhere else
    int tileSize = 16;

    // FIXME: needs better solution
    // the data structure kind of falls apart here
    if (!bitmaps.tiles_loaded || !bitmaps.ui_loaded)
    {
        this_thread::sleep_for(chrono::seconds(5));
        Debug("Unable to rendere because resources not loaded");
        return;
    }

    int* tile = tileMap.idMap;
    for (int y = 0; y < tileMap.rows; y++)
    {
        for (int x = 0; x < tileMap.columns; x++)
        {
            int tileId = *tile++;
            int tileIdx = y * tileMap.columns + x;
            // TODO: use tile size
            int tileX = x * tileSize;
            int tileY = y * tileSize;
            if (tileId == 0)
            {
                DrawBitmap(buffer, bitmaps.sheet[9], tileX, tileY);
            }
            else if (tileId == 1)
            {
                ContextTile tile = tileMap.context_tiles[tileIdx];

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
                DrawBitmap(buffer, bitmaps.sheet[sheetIdx], tileX, tileY);
            }
        }
    }

    // TODO: UI layer
    //  - block mouse over below it?
    //  -

    // draw button
    // TODO: state management for mouse and objects before the draws

    int offset = tileSize / 2;
    int buttonStartX = tileSize * (tileMap.columns - 3) + offset;
    int buttonStartY = tileSize * (tileMap.rows - 2) + offset;
    int buttonEndX = buttonStartX + 2 * tileSize;
    int buttonEndY = buttonStartY + tileSize;

    // Do i want to have a button struct that holds the position info?
    // Probably right? And also how to draw it?
    // Because else it's going to be hard to check against it's position

    BitmapBuffer buttonLeft;
    BitmapBuffer buttonRight;

    // NOTE: with this i can see i'm now getting into the real entity design
    // because now i need a system to check, what is below me
    // and i need to check this for all my Entities
    // - for tiles
    // - for enemies
    // - for ui
    // => And all in the right order
    // >>>> This will be the design challenge for next week!!! <<<<<<<
    bool mouseOverButton;
    if (mouse.x > buttonStartX && mouse.x < buttonEndX &&
        mouse.y > buttonStartY && mouse.y < buttonEndY)
    {
        buttonLeft = bitmaps.uiTiles[10];
        buttonRight = bitmaps.uiTiles[11];
        mouseOverButton = true;
    }
    else
    {

        buttonLeft = bitmaps.uiTiles[8];
        buttonRight = bitmaps.uiTiles[9];
        mouseOverButton = false;
    }

    // draw overlay section stuff
    if (mouse.buttons & MOUSE_LEFT && !mouseOverButton)
    {
        // TODO: tile size information system
        int tileIdxX = mouse.x / tileSize;
        int tileIdxY = mouse.y / tileSize;

        int tileXStart = tileIdxX * tileSize;
        int tileYStart = tileIdxY * tileSize;

        int tileId = tileMap.GetTileId(mouse.x, mouse.y);

        // mockup for blocking placements on the way
        if (tileId == 0)
        {
            DrawBitmap(buffer, bitmaps.uiTiles[0], tileXStart, tileYStart);
        }
        else if (tileId == 1)
        {
            DrawBitmap(buffer, bitmaps.uiTiles[2], tileXStart, tileYStart);
        }
    }

    // draw button stuff
    DrawBitmap(buffer, buttonLeft, buttonStartX, buttonStartY);
    DrawBitmap(buffer, buttonRight, buttonStartX + tileSize, buttonStartY);

    // TODO: save the last state frame based???
    //  else it might skip it no?
    //  - where should the click handling live?
    if (mouse.buttons & MOUSE_RIGHT && !mouseRightLastState)
    {
        mouseRightLastState = !mouseRightLastState;

        if (mouseOverButton)
            PlayAudioFile(&audio.click_hi, false, 90);
        else
            PlayAudioFile(&audio.pop_hi, false, 90);
    }
    else if (~mouse.buttons & MOUSE_RIGHT && mouseRightLastState)
    {
        mouseRightLastState = !mouseRightLastState;
    }

    if (mouse.buttons & MOUSE_LEFT && !mouseLeftLastState)
    {
        mouseLeftLastState = !mouseLeftLastState;

        if (mouseOverButton)
            PlayAudioFile(&audio.click_lo, false, 90);
        else
            PlayAudioFile(&audio.pop_lo, false, 90);
    }
    else if (~mouse.buttons & MOUSE_LEFT && mouseLeftLastState)
    {
        mouseLeftLastState = !mouseLeftLastState;
    }

    // draw mouse
    DrawBitmap(buffer, bitmaps.cursor_sprite, mouse.x, mouse.y, true);
}
