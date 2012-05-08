#ifndef VECTOR2_H
#define VECTOR2_H
#include <math.h>
#include <iostream>
namespace luck { namespace core
{
    template <typename T>
    struct Vector2
    {
        private:
            T vec2[2];
        public:
            T& x;
            T& y;
            Vector2() : x(vec2[0]), y(vec2[1])
            {
                x = (T)0;
                y = (T)0;
            }
            Vector2(T x, T y) : x(vec2[0]), y(vec2[1])
            {
                this->x = x;
                this->y = y;
            }
            Vector2<T>& operator=(Vector2<T> const & c)
            {
                x = c.x;
                y = c.y;
                return *this;
            }
            T& operator[](const u16 n)
            {
                return vec2[n];
            }
    };
}}
#endif

