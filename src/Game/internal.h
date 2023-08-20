#pragma once

#include "../imports.h"

// NOTE: the mouse can only interact with one element at a time
// drag and drop state should go on the ui elements
struct MouseState
{
    int x;
    int y;

    // Is states
    bool left_down;
    bool right_down;

    // event states (this just happened)
    // are only active for one update period
    bool left_clicked;
    bool left_released;
    bool right_clicked;
    bool right_released;
};

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

extern map<TileState, int> PATH_MAP;
extern MouseState mouseState;

void UpdateMouseState();
