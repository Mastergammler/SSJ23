#include "../imports.h"
#include "../types.h"

/**
 * Maps the adjacency state bit orders to a enum
 * Bit order: T L R B TL TR BL BR
 */
enum TileState
{
    SINGLE = 0b00000000,
    MIDDLE = 0b11111111,
    TOP = 0b01110000,
    BOTTOM = 0b11100000,
    LEFT = 0b10110000,
    RIGHT = 0b11010000,
    TL = 0b00110000,
    TR = 0b01010000,
    BL = 0b10100000,
    BR = 0b11000000,
    ITL = 0b11111110,
    ITR = 0b11111101,
    IBL = 0b11111011,
    IBR = 0b11110111
};

/**
 * Maps the tiles to the index of the tile sheet
 */
global_var map<TileState, int> PATH_MAP = {
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
    {IBR, 18}

};
