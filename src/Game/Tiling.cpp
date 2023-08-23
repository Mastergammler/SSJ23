#include "internal.h"

/**
 * Maps the tiles to the index of the tile sheet
 * When the path tiles are the controlling ones
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

const int SHEET_ROWS = 8;
const int SHEET_COLUMNS = 8;
const int SHEET_OFFSET = 3 * SHEET_ROWS;

/**
 * Maps the tiles to the index of the tile sheet for grass tiles
 * When the grass tiles are the controlling ones
 */
map<TileEnv, int> grassMap = {{SINGLE, 12},
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
