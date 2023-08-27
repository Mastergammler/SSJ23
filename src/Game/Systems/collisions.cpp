#include "../internal.h"

// can't return the const element directly, that's why we copy it here
// Not sure if that's the best handling, but ui relies on many places on finding
// an element -> so we want to prevent returning null here
UiElement nullElemCopy = UiElement{.visible = NullElement.visible,
                                   .initialized = NullElement.initialized,
                                   .id = NullElement.id,
                                   .layer = NullElement.layer,
                                   .on_click = NullElement.on_click};

UiElement* FindHighestLayerCollision(int x, int y)
{
    UiElement* foundElement = &nullElemCopy;
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
