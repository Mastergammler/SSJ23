#include "../imports.h"
#include "../types.h"
#include <wingdi.h>

function void RenderBuffer(HDC hdc, ScreenBuffer buffer, Dimension& windowDim)
{
    // destination is where on the window im painting
    // source is where from the buffer im taking the stuff to paint
    // NOTE: destination starts top left
    // buffer starts bottom left
    StretchDIBits(hdc,
                  0,
                  0,
                  windowDim.Width,
                  windowDim.Height,
                  0,
                  0,
                  buffer.width,
                  buffer.height,
                  buffer.memory,
                  &buffer.bitmap_info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

function void RenderNextFrame(HDC hdc, ScreenBuffer buffer, Dimension dim)
{

    // TODO: how to do the actual drawing?
    // this has to be happening in the game stuff itself?
    // DrawTiles(buffer, );
    RenderBuffer(hdc, buffer, dim);
}
