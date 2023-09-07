#include "../internal.h"

Resources Res;

const int SHEET_ROWS = 8;
const int SHEET_COLUMNS = 8;
const int SHEET_OFFSET = 3 * SHEET_ROWS;

void InitStaticResources()
{
    /**
     * Maps the tiles to the index of the tile sheet
     * When the path tiles are the controlling ones
     */
    Game.path_mappings = {// TODO: adjust single
                          {SINGLE, 12},
                          {MIDDLE, 6},
                          {TOP, 4},
                          {BOTTOM, 20},
                          {LEFT, 11},
                          {RIGHT, 13},
                          {TL, 3},
                          {TR, 5},
                          {BL, 19},
                          {BR, 21},
                          {ITL, 0},
                          {ITR, 2},
                          {IBL, 16},
                          {IBR, 18}};
    /**
     * Maps the tiles to the index of the tile sheet for grass tiles
     * When the grass tiles are the controlling ones
     */
    Game.grass_mappings = {{SINGLE, 12},
                           {MIDDLE, 36},
                           {VERTICAL, 6 + SHEET_OFFSET},
                           {HORIZONTAL, 7 + SHEET_OFFSET},
                           {TOP, 4 + SHEET_OFFSET},
                           {TOP_END, 14 + SHEET_OFFSET},
                           {BOTTOM, 20 + SHEET_OFFSET},
                           {BOTTOM_END, 22 + SHEET_OFFSET},
                           {LEFT, 11 + SHEET_OFFSET},
                           {LEFT_END, 15 + SHEET_OFFSET},
                           {RIGHT, 13 + SHEET_OFFSET},
                           {RIGHT_END, 23 + SHEET_OFFSET},
                           {TL, 3 + SHEET_OFFSET},
                           {TR, 5 + SHEET_OFFSET},
                           {BL, 19 + SHEET_OFFSET},
                           {BR, 21 + SHEET_OFFSET},
                           {ITL, 0 + SHEET_OFFSET},
                           {ITR, 2 + SHEET_OFFSET},
                           {IBL, 16 + SHEET_OFFSET},
                           {IBR, 18 + SHEET_OFFSET}};
}

/**
 * Needs to be loaded after the bitmaps, because relies on them!?
 * Nah just the files for now
 */
void LoadItems()
{
    // TODO: do conversion with sounds etc
    ItemArray loaded = LoadItems(ABSOLUTE_RES_PATH + "Items");

    for (int i = 0; i < loaded.count; i++)
    {
        ItemData item = loaded.items[i];
        if (item.loaded)
        {
            // TODO: create entity & items to display
            // and ui items etc etc
        }
    }

    Res.items = loaded;
}

BitmapCache LoadSprites(HINSTANCE hInstance, HDC hdc)
{
    BitmapCache cache = {};

    BitmapBuffer bmp1 = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Cursor.bmp",
                                   hInstance,
                                   hdc);
    if (bmp1.loaded)
    {
        cache.cursor_sprite = bmp1;
    }

    BitmapBuffer tilesSheet = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/"
                                                             "Tiles_8x8.bmp",
                                         hInstance,
                                         hdc);
    if (tilesSheet.loaded)
    {
        cache.ground = ConvertFromSheet(tilesSheet,
                                        Game.tile_size.width,
                                        Game.tile_size.height);
    }

    BitmapBuffer uiSheet = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/UI_8x8.bmp",
                                      hInstance,
                                      hdc);
    if (uiSheet.loaded)
    {
        cache.ui = ConvertFromSheet(uiSheet,
                                    Game.tile_size.width,
                                    Game.tile_size.height);
    }

    BitmapBuffer charactersSheet = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/"
                                                                  "Characters_"
                                                                  "8x8.bmp",
                                              hInstance,
                                              hdc);
    if (charactersSheet.loaded)
    {
        cache.characters = ConvertFromSheet(charactersSheet,
                                            Game.tile_size.width,
                                            Game.tile_size.height);
    }

    BitmapBuffer itemsSheet = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/"
                                                             "Items_8x8.bmp",
                                         hInstance,
                                         hdc);
    if (itemsSheet.loaded)
    {
        cache.items = ConvertFromSheet(itemsSheet,
                                       Game.tile_size.width,
                                       Game.tile_size.height);
    }

    BitmapBuffer logo = LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Logo.bmp",
                                   hInstance,
                                   hdc);

    if (logo.loaded)
    {
        cache.logo = logo;
    }

    return cache;
}

SoundCache LoadAudioFiles()
{
    SoundCache cache = {};

    WaveBuffer wave = LoadWaveFile(ABSOLUTE_RES_PATH + "Music/TitleTheme.wav");
    if (wave.loaded)
    {
        cache.music = wave;
    }

    // Sounds
    WaveBuffer clickLo = LoadWaveFile(ABSOLUTE_RES_PATH +
                                      "Sounds/Click-Lo.wav");
    if (clickLo.loaded)
    {
        cache.click_lo = clickLo;
    }
    WaveBuffer clickHi = LoadWaveFile(ABSOLUTE_RES_PATH +
                                      "Sounds/Click-Hi.wav");
    if (clickHi.loaded)
    {
        cache.click_hi = clickHi;
    }
    WaveBuffer popLo = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Lo.wav");
    if (popLo.loaded)
    {
        cache.pop_lo = popLo;
    }

    WaveBuffer popHi = LoadWaveFile(ABSOLUTE_RES_PATH + "Sounds/Pop-Hi.wav");
    if (popHi.loaded)
    {
        cache.pop_hi = popHi;
    }
    WaveBuffer craftError = LoadWaveFile(ABSOLUTE_RES_PATH +
                                         "Sounds/Craft-Error.wav");
    if (craftError.loaded)
    {
        cache.craft_error = craftError;
    }
    WaveBuffer craftSuccess = LoadWaveFile(ABSOLUTE_RES_PATH +
                                           "Sounds/Craft-Success.wav");
    if (craftSuccess.loaded)
    {
        cache.craft_success = craftSuccess;
    }
    return cache;
}

void DetermineAdjacents(TileMap map, int tileIndex)
{
    bool left = map.sameHorizontally(tileIndex, tileIndex - 1);
    bool right = map.sameHorizontally(tileIndex, tileIndex + 1);
    bool top = map.sameVertically(tileIndex, tileIndex - map.columns);
    bool bot = map.sameVertically(tileIndex, tileIndex + map.columns);
    bool tl = map.areSameAllDir(tileIndex, tileIndex - map.columns - 1);
    bool tr = map.areSameAllDir(tileIndex, tileIndex - map.columns + 1);
    bool bl = map.areSameAllDir(tileIndex, tileIndex + map.columns - 1);
    bool br = map.areSameAllDir(tileIndex, tileIndex + map.columns + 1);

    BYTE adjacents = (top << 7) | (left << 6) | (right << 5) | (bot << 4) |
                     (tl << 3) | (tr << 2) | (bl << 1) | (br << 0);

    int id = map.tiles[tileIndex].tile_id;

    if (id == 5)
    {
        cout << "Adjacents for tile: " << tileIndex << "(" << id << ") is "
             << bitset<8>(adjacents) << endl;
    }

    map.tiles[tileIndex].adjacent = adjacents;
}

const char SPAWN_VALUE = 's';
const char TARGET_VALUE = 't';
const int ASCII_LETTER_OFFSET = 32;

TileType fromNumber(int num)
{
    switch (num)
    {
        case GRASS_TILE: return GRASS_TILE;
        case PATH_TILE: return PATH_TILE;
        default: return GRASS_TILE;
    }
}

// TODO: i really want hot reload right now, to see what changes on different
// maps
//  - this would be super helpful
//  - also great for ppl trying it out?!
TileMap LoadTilemap(string mapPath)
{
    TileMapRaw rawMap = LoadMap(mapPath);

    Tile* tiles = new Tile[rawMap.rows * rawMap.columns];
    Tile* tmp = tiles;

    int endTileCount = 0;
    int startTileCount = 0;

    char* values = rawMap.data;
    for (int row = 0; row < rawMap.rows; row++)
    {
        for (int col = 0; col < rawMap.columns; col++)
        {
            Tile tile = Tile{};
            tile.tracker = new EntityTracker{};
            tile.tracker->entity_ids = new int[tile.tracker->MAX_COUNT];

            char val = *values++;

            tile.pos.x = col;
            tile.pos.y = row;

            // start and end are on the path as well
            if (val == SPAWN_VALUE ||
                val == (SPAWN_VALUE - ASCII_LETTER_OFFSET))
            {
                tile.is_start = true;
                tile.tile_id = PATH_TILE;
                startTileCount++;
            }
            else if (val == TARGET_VALUE ||
                     val == (TARGET_VALUE - ASCII_LETTER_OFFSET))
            {
                tile.is_end = true;
                tile.tile_id = PATH_TILE;
                endTileCount++;
            }
            else
            {
                // calculating ascii offset
                int numericalVal = val - '0';
                tile.tile_id = fromNumber(numericalVal);
            }

            *tmp++ = tile;
        }
    }

    TileMap map = {};
    map.columns = rawMap.columns;
    map.rows = rawMap.rows;
    map.tile_size = Game.tile_size;
    map.tile_count = map.columns * map.rows;
    map.tiles = tiles;
    map.min_pos = {0, 0};
    map.max_pos = {map.columns - 1, map.rows - 1};
    map.spawn_count = startTileCount;
    map.target_count = endTileCount;

    // TODO: validate that start and finish exists?
    map.spawns = new Tile*[map.spawn_count];
    map.targets = new Tile*[map.target_count];

    Tile** curSpawn = map.spawns;
    Tile** curTarget = map.targets;

    for (int i = 0; i < map.rows * map.columns; i++)
    {
        DetermineAdjacents(map, i);

        Tile* cur = &tiles[i];
        if (cur->is_end)
        {
            *curTarget++ = cur;
        }
        else if (cur->is_start)
        {
            *curSpawn++ = cur;
        }
    }

    return map;
}

SpriteSheet ConvertFromSheet(BitmapBuffer sheetBitmap,
                             int tileWidth,
                             int tileHeight)
{
    u32* bitmapPixels = (u32*)sheetBitmap.buffer;

    // ignore clipping, uneven counts are ignored
    int tilesPerRow = sheetBitmap.width / tileWidth;
    int tilesPerColumn = sheetBitmap.height / tileHeight;
    int tileCount = tilesPerRow * tilesPerColumn;
    int pixelsPerTile = tileWidth * tileHeight;

    BitmapBuffer* bitmaps = new BitmapBuffer[tileCount];

    for (int tileIdx = 0; tileIdx < tileCount; tileIdx++)
    {
        int tileRow = tileIdx / tilesPerRow;
        int tileCol = tileIdx % tilesPerRow;

        int tilePixelIndex = (sheetBitmap.width * tileHeight) * tileRow +
                             tileCol * tileWidth;
        u32* tileStartPixel = (u32*)sheetBitmap.buffer;
        tileStartPixel += tilePixelIndex;

        u32* tilePixels = new u32[pixelsPerTile];
        u32* tilePointer = tilePixels;

        // Inverse saving because i want to specify numbers from the UL
        int mirrorIdx = MirrorIndex(tileIdx, tilesPerRow, tilesPerColumn);
        bitmaps[mirrorIdx] = BitmapBuffer{tileWidth, tileHeight, tilePointer};

        for (int y = 0; y < tileHeight; y++)
        {
            u32* pixelRow = tileStartPixel + (y * sheetBitmap.width);
            for (int x = 0; x < tileWidth; x++)
            {
                *tilePixels++ = *pixelRow++;
            }
        }
    }

    SpriteSheet sheet = {};
    sheet.tiles = bitmaps;
    sheet.loaded = true;
    sheet.rows = tilesPerColumn;
    sheet.columns = tilesPerRow;
    sheet.image_width = sheetBitmap.width;
    sheet.image_height = sheetBitmap.height;
    sheet.tile_count = tileCount;
    sheet.tile_width = tileWidth;
    sheet.tile_height = tileHeight;

    return sheet;
}
