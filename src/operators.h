#pragma once

#include "types.h"

function v2 operator+(const v2 first, const v2 second)
{
    return {first.x + second.x, first.y + second.y};
};

function v2 operator-(const v2 first, const v2 second)
{
    return {first.x - second.x, first.y - second.y};
}

function v2 operator*(const v2 v1, const v2 v2)
{
    return {v1.x * v2.x, v1.y * v2.y};
}

function v2 operator*(int magnitude, const v2 vector)
{
    return {vector.x * magnitude, vector.y * magnitude};
};

function prox2 operator*(float magnitude, const prox2 vector)
{
    return {vector.x * magnitude, vector.y * magnitude};
}

function prox2 operator*(float magnitude, const v2 vector)
{
    return {vector.x * magnitude, vector.y * magnitude};
}
