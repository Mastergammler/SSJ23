#include "internal.h"
#include "module.h"

Mouse mouse = {};

void HandleMouseMove(LPARAM eventInfo)
{
    // update would happen via a poll update
    // because we're checking every frame
    // because it's not an event based architecture
    // TEST: is this correct now using the correct window scale?
    int windowHeight = Scale.screen_dim.height;

    int actualScreenX = LOWORD(eventInfo);
    int untreatedY = HIWORD(eventInfo);
    int actualScreenY = (windowHeight - 1 - HIWORD(eventInfo));

    float screenPercX = (float)actualScreenX / Scale.screen_dim.width;
    float screenPercY = (float)actualScreenY / Scale.screen_dim.height;

    int bufferX = Scale.render_dim.width * screenPercX;
    int bufferY = Scale.render_dim.height * screenPercY;

    mouse.x = bufferX;
    mouse.y = bufferY;
}

void HandleMouseButton(MouseButtons button, bool pressed)
{
    if (pressed)
    {
        mouse.buttons |= button;
    }
    else
    {
        mouse.buttons &= ~button;
    }
}
