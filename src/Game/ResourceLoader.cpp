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
            ConvertFromSheet(tilesSheet, TileSize.width, TileSize.height);
    }

    BitmapBuffer uiSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/UI_8x8.bmp", hInstance, hdc);
    if (uiSheet.loaded)
    {
        cache.ui = ConvertFromSheet(uiSheet, TileSize.width, TileSize.height);
    }

    BitmapBuffer charactersSheet =
        LoadSprite(ABSOLUTE_RES_PATH + "Sprites/Characters_8x8.bmp",
                   hInstance,
                   hdc);
    if (charactersSheet.loaded)
    {
        cache.characters =
            ConvertFromSheet(charactersSheet, TileSize.height, TileSize.width);
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

Tilemap LoadMaps()
{
    return LoadMap(ABSOLUTE_RES_PATH + "Test/Tilemap_15_20.map");
}
