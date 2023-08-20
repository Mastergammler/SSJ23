#include "internal.h"
#include "module.h"

Mouse mouse = {};

void HandleMouseMove(LPARAM eventInfo)
{
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
