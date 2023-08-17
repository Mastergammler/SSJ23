#include "../globals.h"
#include "../imports.h"
#include "../types.h"

struct Mouse
{
    int x, y;
    u8 buttons;
} mouse;

enum MouseButtons
{
    MOUSE_LEFT = 0b1,
    MOUSE_MIDDLE = 0b10,
    MOUSE_RIGHT = 0b100,
    MOUSE_X1 = 0b1000,
    MOUSE_X2 = 0b10000
};

void HandleMouseMove(LPARAM eventInfo)
{
    if (cursor) { SetCursor(cursor); }

    // update would happen via a poll update
    // because we're checking every frame
    // because it's not an event based architecture
    int windowHeight = scale.screen_height;

    int actualScreenX = LOWORD(eventInfo);
    int untreatedY = HIWORD(eventInfo);
    int actualScreenY = (windowHeight - 1 - HIWORD(eventInfo));

    float screenPercX = (float)actualScreenX / scale.screen_width;
    float screenPercY = (float)actualScreenY / scale.screen_height;

    int bufferX = scale.draw_width * screenPercX;
    int bufferY = scale.draw_height * screenPercY;

    mouse.x = bufferX;
    mouse.y = bufferY;
}

void HandleMouseButton(MouseButtons button, bool pressed)
{
    if (pressed) { mouse.buttons |= button; }
    else { mouse.buttons &= ~button; }
}
