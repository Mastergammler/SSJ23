#include "../IO/module.h"
#include "../Rendering/module.h"
#include "../Win32/module.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

struct Bitmaps
{
    BitmapBuffer tile_a = {};
    BitmapBuffer tile_b = {};

    int tile_count = 0;
    bool sheet_loaded = false;
    BitmapBuffer* sheet = {};

    void SetSheet(BitmapBuffer* sheet)
    {
        sheet_loaded = true;
        this->sheet = sheet;
    }
};

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";
global_var Bitmaps bitmaps = {};

global_var const int TILE_ROWS = 12;
global_var const int TILE_COLUMNS = 15;
global_var Tilemap map = {0, 0};

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    Log(logger, "Start loading resources ...");

    BitmapBuffer bitmap =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/TileTest.bmp", hInstance, hdc);
    if (bitmap.loaded) { bitmaps.tile_a = bitmap; }
    BitmapBuffer bmp2 =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/TileTest_2.bmp", hInstance, hdc);
    if (bmp2.loaded) { bitmaps.tile_b = bmp2; }

    BitmapBuffer tilesSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Tiles.bmp", hInstance, hdc);
    if (tilesSheet.loaded)
    {
        Log(logger, "Start converting sprite sheet");
        BitmapBuffer* tiles = ConvertFromSheet(tilesSheet, 16, 16);
        bitmaps.SetSheet(tiles);
    }

    Log(logger, "Game Resources loaded");

    map = LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");

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
}

/**
 * Called to update the information and buffer for the next frame
 */
void UpdateScreen(ScreenBuffer& buffer)
{
    // TODO: save tile sive somewhere else
    int tileSize = 16;

    // FIXME: needs better solution
    // the data structure kind of falls apart here
    if (!bitmaps.sheet_loaded)
    {
        this_thread::sleep_for(chrono::seconds(5));
        Debug("Unable to rendere because resources not loaded");
        return;
    }

    int* tile = map.idMap;
    for (int y = 0; y < map.rows; y++)
    {
        for (int x = 0; x < map.columns; x++)
        {
            int tileId = *tile++;
            // TODO: use tile size
            int tileX = x * tileSize;
            int tileY = y * tileSize;
            if (tileId == 0)
            {
                DrawTile(buffer, bitmaps.sheet[8], tileX, tileY);
            }
            else if (tileId == 1)
            {
                DrawTile(buffer, bitmaps.sheet[9], tileX, tileY);
            }
        }
    }

    // TODO: do i need to scale it? Because the window is bigger??
    if (mouse.buttons & MOUSE_LEFT)
    {
        DrawRect(buffer, mouse.x, mouse.y, 8, true);
    }
}
