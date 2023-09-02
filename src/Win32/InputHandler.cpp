#include "internal.h"
#include "module.h"

Mouse mouse = {};

//  BUG: for scale 2 1 etc the mouse position is wrong
//  because static bug -> don't use static, when using module stuff
void HandleMouseMove(LPARAM eventInfo)
{
    // update would happen via a poll update
    // because we're checking every frame
    // because it's not an event based architecture
    int windowHeight = Scale.screen_height;

    int actualScreenX = LOWORD(eventInfo);
    int untreatedY = HIWORD(eventInfo);
    int actualScreenY = (windowHeight - 1 - HIWORD(eventInfo));

    float screenPercX = (float)actualScreenX / Scale.screen_width;
    float screenPercY = (float)actualScreenY / Scale.screen_height;

    int bufferX = Scale.draw_width * screenPercX;
    int bufferY = Scale.draw_height * screenPercY;

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
