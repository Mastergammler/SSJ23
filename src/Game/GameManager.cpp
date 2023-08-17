#include "../IO/module.h"
#include "../Rendering/module.h"
#include "../Win32/module.h"
#include "../imports.h"
#include "../types.h"
#include "../utils.h"
#include "Tiling.cpp"

struct Bitmaps
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

// have 2 maps, one per tileId
// and this map has as key the adjacent thing

// TODO: how to get the real execution path? and resource directories?
// how to copy the stuff?
global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";
global_var Bitmaps bitmaps = {};

global_var const int TILE_ROWS = 12;
global_var const int TILE_COLUMNS = 15;
global_var Tilemap tileMap = {0, 0, 0};

/**
 * Loads all the games resources.
 * Required to be called before the first update can be called
 */
void InitGame(HINSTANCE hInstance, HDC hdc)
{
    Log(logger, "Start loading resources ...");

    BitmapBuffer bmp1 =
        LoadSprite(ABSOLUTE_RES_PATH + "Test/CursorTest.bmp", hInstance, hdc);
    if (bmp1.loaded) { bitmaps.cursor_sprite = bmp1; }
    BitmapBuffer bmp2 = LoadSprite(ABSOLUTE_RES_PATH + "Test/TileHighlight.bmp",
                                   hInstance,
                                   hdc);
    if (bmp2.loaded) { bitmaps.tile_highlight = bmp2; }

    BitmapBuffer tilesSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Tiles_8x8.bmp", hInstance, hdc);
    if (tilesSheet.loaded)
    {
        // TODO: tile size info
        BitmapBuffer* tiles = ConvertFromSheet(tilesSheet, 16, 16);
        bitmaps.SetSheet(tiles);
    }

    BitmapBuffer uiSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "UI_8x8.bmp", hInstance, hdc);
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

    tileMap = LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");

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
                DrawTile(buffer, bitmaps.sheet[9], tileX, tileY);
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
                DrawTile(buffer, bitmaps.sheet[sheetIdx], tileX, tileY);
            }
        }
    }

    if (mouse.buttons & MOUSE_LEFT)
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
            DrawTile(buffer, bitmaps.uiTiles[0], tileXStart, tileYStart);
        }
        else if (tileId == 1)
        {
            DrawTile(buffer, bitmaps.uiTiles[2], tileXStart, tileYStart);
        }
    }

    // draw mouse
    DrawTile(buffer, bitmaps.cursor_sprite, mouse.x, mouse.y);
}
