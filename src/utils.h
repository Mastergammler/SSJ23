#pragma once

#include "types.h"

/**
 * Returns a random between  0 and the max value
 * (both inclusive)
 */
function int RndInt(int max)
{
    assert(max > 0);
    return rand() % (max + 1);
}

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
 * Clamps to the floor of a certain value
 * -> Means no value can be lower than the floor
 */
function int Floor(int value, int floor)
{
    return value < floor ? floor : value;
}

/**
 * Clamps to a certain ceiling of a value
 * Means no value can be higher than the ceiling
 */
function int Ceiling(int value, int ceiling)
{
    return value > ceiling ? ceiling : value;
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
