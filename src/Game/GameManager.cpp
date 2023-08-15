#include "../IO/module.h"
#include "../Rendering/module.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"

struct Bitmaps
{
    BitmapBuffer tile_a = {};
    BitmapBuffer tile_b = {};
};

struct Tilemap
{
    int rows;
    int columns;
    int* idMap;
    int tilemapInit[5][15];

    Tilemap(int rows, int columns)
    {
        this->rows = rows;
        this->columns = columns;

        int size = sizeof(int) * rows * columns;
        // this->idMap = (int*) VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE,
        // PAGE_READWRITE);
    }
};

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";
global_var Bitmaps bitmaps = {};
global_var Tilemap map = {5, 15};

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    BitmapBuffer bitmap =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/TileTest.bmp", hInstance, hdc);
    if (bitmap.loaded) { bitmaps.tile_a = bitmap; }
    BitmapBuffer bmp2 =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/TileTest_2.bmp", hInstance, hdc);
    if (bmp2.loaded) { bitmaps.tile_b = bmp2; }

    Log(logger, "Game Resources loaded");

    // FIXME: this is independant from the map init ....
    int tilemapInit[5][15] = {
        {0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    };

    memcpy(map.tilemapInit, tilemapInit, sizeof(tilemapInit));
    map.idMap = &map.tilemapInit[0][0];

    // map.idMap = &tilemapInit[0][0];
    // int* idm = &tilemapInit[0][0];
    // map.idMap = idm;

    // int* idm = map.idMap;
    for (int y = 0; y < map.rows; y++)
    {
        for (int x = 0; x < map.columns; x++)
        {
            //       *idm++ = (x + y) % 2;
            cout << "Assigning values: " << x << "|" << y << " = "
                 << tilemapInit[x][y] << endl;
        }
    }
}

/**
 * Called to update the information and buffer for the next frame
 */
void Update(ScreenBuffer& buffer)
{

    int* tile = map.idMap;
    for (int y = 0; y < map.rows; y++)
    {
        for (int x = 0; x < map.columns; x++)
        {
            int tileId = *tile++;
            // TODO: use tile size
            int tileX = x * 32;
            int tileY = y * 32;
            if (tileId == 1) { DrawTile(buffer, bitmaps.tile_a, tileX, tileY); }
            else if (tileId == 0)
            {
                DrawTile(buffer, bitmaps.tile_b, tileX, tileY);
            }
        }
    }

    // Update buffer info
    // DrawTiles(screen, bitmaps.tile_b);
}
