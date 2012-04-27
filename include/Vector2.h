#ifndef VECTOR2_H
#define VECTOR2_H
#include <math.h>
#include <iostream>
namespace luck { namespace core
{
    template <typename T>
    struct Vector2
    {
        Vector2()
        {
            x = (T)0;
            y = (T)0;
        }
        Vector2(T x, T y)
        {
            this->x = x;
            this->y = y;
        }
        T x;
        T y;
    };
}}
#endif

