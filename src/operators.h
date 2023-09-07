#pragma once

#include "types.h"

v2 operator+(const v2 first, const v2 second)
{
    return {first.x + second.x, first.y + second.y};
};

v2 operator*(const v2 v1, const v2 v2)
{
    return {v1.x * v2.x, v1.y * v2.y};
}

v2 operator*(int magnitude, const v2 vector)
{
    return {vector.x * magnitude, vector.y * magnitude};
};
