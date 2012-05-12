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
        f32 r;
        f32 g;
        f32 b;
        f32 a;
        Vertex(f32 x, f32 y, f32 z, u8 r, u8 g, u8 b, u8 a) : x(x), y(y), z(z), r(r), g(g), b(b), a(a)
        { }
        Vertex(Vector3<f32> position, Color4 color) :
            x(position.x), y(position.y), z(position.z), r(color.r), g(color.g), b(color.b), a(color.a)
        { }
        Vertex() = default;
    };
}}
#endif

