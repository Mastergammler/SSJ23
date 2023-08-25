#include "../internal.h"

// TODO: use this null element, or another one?
//  the default one as 0 ??
UiElement NullElement = {.visible = false,
                         .initialized = false,
                         .id = -1,
                         .layer = -1,
                         .on_click = [] {}};

UiElement* FindHighestLayerCollision(int x, int y)
{
    UiElement* foundElement = &NullElement;
    for (int i = 0; i < uiElements.count; ++i)
    {
        UiElement* cur = &uiElements.elements[i];
        if (cur->visible)
        {
            if (cur->x_start <= x && cur->x_end >= x && cur->y_start <= y &&
                cur->y_end >= y)
            {
                if (cur->layer > foundElement->layer)
                {
                    foundElement = cur;
                }
            }
        }
    }

    return foundElement;
}
