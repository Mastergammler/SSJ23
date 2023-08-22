#include "internal.h"
#include "module.h"

global_var const string ABSOLUTE_RES_PATH = "I:/02 Areas/Dev/Cpp/SSJ23/res/";

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc)
{
    BitmapCache cache = {};

    BitmapBuffer bmp1 =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Cursor.bmp", hInstance, hdc);
    if (bmp1.loaded) { cache.cursor_sprite = bmp1; }
    BitmapBuffer bmp2 = LoadSprite(ABSOLUTE_RES_PATH + "Test/TileHighlight.bmp",
                                   hInstance,
                                   hdc);
    if (bmp2.loaded) { cache.tile_highlight = bmp2; }

    BitmapBuffer tilesSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Tiles_8x8.bmp", hInstance, hdc);
    if (tilesSheet.loaded)
    {
        cache.ground =
            ConvertFromSheet(tilesSheet, tileSize.width, tileSize.height);
    }

    BitmapBuffer uiSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/UI_8x8.bmp", hInstance, hdc);
    if (uiSheet.loaded)
    {
        cache.ui = ConvertFromSheet(uiSheet, tileSize.width, tileSize.height);
    }

    BitmapBuffer charactersSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Characters_8x8.bmp",
                   hInstance,
                   hdc);
    if (charactersSheet.loaded)
    {
        cache.characters =
            ConvertFromSheet(charactersSheet, tileSize.height, tileSize.width);
    }

    return cache;
}

SoundCache LoadAudioFiles()
{
    SoundCache cache = {};

    WaveBuffer wave = LoadWaveFile(ABSOLUTE_RES_PATH + "Music/TitleTheme.wav");
    if (wave.loaded) { cache.music = wave; }

    // Sounds
    WaveBuffer clickLo =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Lo.wav");
    if (clickLo.loaded) { cache.click_lo = clickLo; }
    WaveBuffer clickHi =
        LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Click-Hi.wav");
    if (clickHi.loaded) { cache.click_hi = clickHi; }
    WaveBuffer popLo = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Lo.wav");
    if (popLo.loaded) { cache.pop_lo = popLo; }
    WaveBuffer popHi = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Hi.wav");
    if (popHi.loaded) { cache.pop_hi = popHi; }

    return cache;
}

void DetermineAdjacents(TileMap map, int tileIndex)
{
    bool left = map.IsSameHor(tileIndex, tileIndex - 1);
    bool right = map.IsSameHor(tileIndex, tileIndex + 1);
    bool top = map.IsSameVert(tileIndex, tileIndex + map.columns);
    bool bot = map.IsSameVert(tileIndex, tileIndex - map.columns);
    bool tl = map.IsSameBoth(tileIndex, tileIndex + map.columns - 1);
    bool tr = map.IsSameBoth(tileIndex, tileIndex + map.columns + 1);
    bool bl = map.IsSameBoth(tileIndex, tileIndex - map.columns - 1);
    bool br = map.IsSameBoth(tileIndex, tileIndex - map.columns + 1);

    BYTE adjacents = (top << 7) | (left << 6) | (right << 5) | (bot << 4) |
                     (tl << 3) | (tr << 2) | (bl << 1) | (br << 0);

    int id = map.tiles[tileIndex].tile_id;

    if (id == 5)
    {
        // cout << "Adjacents for tile: " << idx << "(" << id << ") is "
        //     << bitset<8>(adjacents) << endl;
    }

    // TODO: is this correct or do i need to dereference?
    // -> should be
    map.tiles[tileIndex].adjacent = adjacents;
}

const char SPAWN_VALUE = 's';
const char TARGET_VALUE = 't';
const int PATH_TILE_ID = 1;

TileMap LoadMaps()
{
    TileMapRaw rawMap = LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");

    Tile* tiles = new Tile[rawMap.rows * rawMap.columns];
    Tile* tmp = tiles;

    char* values = rawMap.data;
    for (int row = 0; row < rawMap.rows; row++)
    {
        for (int col = 0; col < rawMap.columns; col++)
        {
            Tile tile = Tile{};
            char val = *values++;

            // start and end are on the path as well
            if (val == SPAWN_VALUE)
            {
                tile.is_start = true;
                tile.tile_id = PATH_TILE_ID;
            }
            else if (val == TARGET_VALUE)
            {
                tile.is_end = true;
                tile.tile_id = PATH_TILE_ID;
            }
            else
            {
                // calculating ascii offset
                tile.tile_id = val - '0';
            }

            *tmp++ = tile;
        }
    }

    TileMap map = {};
    map.columns = rawMap.columns;
    map.rows = rawMap.rows;
    map.tile_size = tileSize;
    map.tile_count = map.columns * map.rows;
    map.tiles = tiles;

    for (int i = 0; i < map.rows * map.columns; i++)
    {
        DetermineAdjacents(map, i);
    }

    return map;
}
