#ifndef VERTEX_H
#define VERTEX_H
#include "luck.h"
#include "Vector3.h"
#include "Color4.h"
namespace luck { namespace core
{
    struct Vertex
    {
        f32 x;
        f32 y;
        f32 z;
        u8 a;
        u8 r;
        u8 g;
        u8 b;
        Vertex(f32 x, f32 y, f32 z, u8 r, u8 g, u8 b, u8 a) : x(x), y(y), z(z), a(a), r(r), g(g), b(b)
        { }
        Vertex(Vector3<f32> position, Color4 color) :
            x(position.x), y(position.y), z(position.z), a(color.a), r(color.r), g(color.g), b(color.b)
        { }
        Vertex() = default;
    };
}}
#endif

