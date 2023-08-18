#pragma once

#include "imports.h"

#define global_var static
#define function static

typedef unsigned int u32;
typedef uint8_t u8;
typedef uint16_t u16;

struct Dimension
{
    int Width;
    int Height;
};

struct ScreenBuffer
{
    ScreenBuffer(int width, int height)
    {
        this->width = width;
        this->height = height;
        this->size = height * width * sizeof(u32);
        this->pixel_count = height * width;

        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
    }

    void* memory;
    int width;
    int height;
    int pixel_count;
    int size;
    BITMAPINFO bitmap_info;
};

struct WindowScale
{
  private:
    int window_height;

  public:
    int scale;
    int draw_width;
    int draw_height;
    int screen_width;
    int screen_height;

    WindowScale(int drawWidth, int drawHeight, int scale)
    {
        this->scale = scale;
        this->draw_width = drawWidth;
        this->draw_height = drawHeight;

        this->screen_width = drawWidth * scale;
        this->screen_height = drawHeight * scale;
    }

    int GetWindowHeight()
    {
        if (!window_height) return screen_height;
        return window_height;
    }

    void AdjustForTaskbarHeight(Dimension& drawableScreen)
    {
        int taskbarHeight = this->screen_height - drawableScreen.Height;
        cout << "Taskbarheight is " << taskbarHeight << endl;
        drawableScreen.Height = this->screen_height;
        this->window_height = this->screen_height + taskbarHeight;
    }
};

struct Rect
{
    int x1, x2, y1, y2;

    Rect(int x, int y, int width, int height)
    {
        this->x1 = x;
        this->y1 = y;
        this->x2 = x + width;
        this->y2 = y + height;
    }
};

struct ContextTile
{
    int tileId;

    /**
     * Bit-Order T L R B TL TR BL BR
     */
    BYTE adjacent;
};

/**
 * If the boundary should count as another some tile there or not
 */
const bool BOUNDARY_DEFAULT = true;

struct Tilemap
{
    int rows;
    int columns;
    int* idMap;
    int tile_size;
    ContextTile* context_tiles;

    Tilemap(const int rows, const int columns, int tile_size)
    {
        this->rows = rows;
        this->columns = columns;
        this->idMap = new int[rows * columns];
        this->tile_size = tile_size;
        this->context_tiles = new ContextTile[rows * columns];
    }

    /**
     * Size has to match the one in the map
     * Flips the map horizontally, for rendering
     * Because rendering stars LL but for definition UL is used
     */
    void AssignMap(int* loadedMap)
    {
        int* tile = idMap;
        int* mapStart = loadedMap;

        for (int y = rows - 1; y >= 0; y--)
        {
            int* rowStart = mapStart + y * columns;
            for (int x = 0; x < columns; x++)
            {
                *tile++ = *rowStart++;
            }
        }

        for (int i = 0; i < rows * columns; i++)
        {
            CreateContextTile(i);
        }
    }

    // TODO: not happy with this
    // looks inefficient (but doesn't matter is load only)
    void CreateContextTile(int idx)
    {
        int curTile = idx;
        int tileRow = curTile % columns;

        bool left = IsSameHor(curTile, curTile - 1);
        bool right = IsSameHor(curTile, curTile + 1);
        bool top = IsSameVert(curTile, curTile + columns);
        bool bot = IsSameVert(curTile, curTile - columns);
        bool tl = IsSameBoth(curTile, curTile + columns - 1);
        bool tr = IsSameBoth(curTile, curTile + columns + 1);
        bool bl = IsSameBoth(curTile, curTile - columns - 1);
        bool br = IsSameBoth(curTile, curTile - columns + 1);

        BYTE adjacents = (top << 7) | (left << 6) | (right << 5) | (bot << 4) |
                         (tl << 3) | (tr << 2) | (bl << 1) | (br << 0);
        int id = idMap[curTile];

        if (id == 5)
        {
            cout << "Adjacents for tile: " << idx << "(" << id << ") is "
                 << bitset<8>(adjacents) << endl;
        }

        ContextTile contextTile = {};
        contextTile.tileId = idMap[curTile];
        contextTile.adjacent = adjacents;
        context_tiles[curTile] = contextTile;
    }

    bool IsSameHor(int tileIdx, int otherIdx)
    {
        // of not same row, means we're at either boundary
        if (TileRowOf(tileIdx) == TileRowOf(otherIdx))
        {
            return SameTile(tileIdx, otherIdx);
        }

        // Boundry case
        return BOUNDARY_DEFAULT;
    }

    bool IsSameVert(int tileIdx, int otherIdx)
    {
        int tileRow = TileRowOf(tileIdx);
        int otherRow = TileRowOf(otherIdx);

        if (tileRow == 0 && otherRow < tileRow) return BOUNDARY_DEFAULT;
        if (tileRow == rows - 1 && otherRow > tileRow) return BOUNDARY_DEFAULT;

        return SameTile(tileIdx, otherIdx);
    }

    // difference - row has to be different!
    // but only difference of one
    bool IsSameBoth(int tileIdx, int otherIdx)
    {
        int diff = TileRowOf(tileIdx) - TileRowOf(otherIdx);
        int normalized = abs(diff);

        // it has to be one row above or below the current row
        if (normalized != 1) return BOUNDARY_DEFAULT;
        return IsSameVert(tileIdx, otherIdx);
    }

    bool SameTile(int tileIdx, int otherIdx)
    {
        if (otherIdx < 0) return BOUNDARY_DEFAULT;
        assert(tileIdx >= 0 && "Tile index must be grater than 0");

        int* tileIds = idMap;

        int thisTileId = tileIds[tileIdx];
        int otherTileId = tileIds[otherIdx];

        return thisTileId == otherTileId;
    }

    int TileRowOf(int idx)
    {
        if (idx < 0) return -1;

        return idx / columns;
    }

    int GetTileId(int screenX, int screenY)
    {
        int tileX = screenX / tile_size;
        int tileY = screenY / tile_size;

        int tileIdx = tileY * columns + tileX;

        return idMap[tileIdx];
    }
};

// TODO: not sure if thats the way to go, but lets see
#pragma pack(push, 1)
struct Sample24
{
    unsigned int value : 24;
};
#pragma pack(pop)

struct WaveBuffer
{
    u32 sample_count;
    u16* data;
    bool loaded;
};
