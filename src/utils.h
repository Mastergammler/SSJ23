#pragma once

#include "types.h"

function void Debug(string msg) { cout << "[Debug] " << msg << endl; }

/**
 * Takes a index that grows from LL to UR and transfroms it
 * Into a index that grows from UL to LR
 *
 * That means the rows are mirrored, but the columns stay the same
 *
 * 4 5 6 7
 * 0 1 2 3
 *
 * becomes
 *
 * 0 1 2 3
 * 4 5 6 7
 *
 * For reasons of readability
 * (Maybe drawing from UL to LR is better in any case?)
 */
function int MirrorIndex(int index, int totalColumns, int totalRows)
{
    int columnIdx = index % totalColumns;
    int rowIdx = index / totalColumns;
    int rowGrowth = (totalRows - 1) - rowIdx;

    int mirroredIndex = rowGrowth * totalColumns + columnIdx;

    return mirroredIndex;
}
