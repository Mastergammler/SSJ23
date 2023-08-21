#include "internal.h"

/**
 * Maps the tiles to the index of the tile sheet
 */
map<TileEnv, int> pathMap = {
    // TODO: adjust single
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
