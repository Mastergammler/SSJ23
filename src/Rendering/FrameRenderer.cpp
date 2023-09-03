#include "module.h"

void RenderFrame(HDC hdc, ScreenBuffer buffer, Dimension dim)
{
    // destination is where on the window im painting
    // source is where from the buffer im taking the stuff to paint
    // NOTE: destination starts top left
    // buffer starts bottom left
    StretchDIBits(hdc,
                  0,
                  0,
                  dim.width,
                  dim.height,
                  0,
                  0,
                  buffer.width,
                  buffer.height,
                  buffer.memory,
                  &buffer.bitmap_info,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}
