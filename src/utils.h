#pragma once

#include "types.h"

function void Debug(string msg)
{
    cout << "[Debug] " << msg << endl;
}

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

function int MirrorY(int y, int height)
{
    return height - 1 - y;
}

/**
 * Returns the value if it is bigger than the min, else the minimum
 * Clamps to the minimum
 */
function int Lower(int value, int min)
{
    return value < min ? min : value;
}

/**
 * Clamps to the maximum
 */
function int Upper(int value, int max)
{
    return value > max ? max : value;
}

function int Clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

function v2 Clamp(v2 value, v2 min, v2 max)
{
    value.x = Clamp(value.x, min.x, max.x);
    value.y = Clamp(value.y, min.y, max.y);

    return value;
}
