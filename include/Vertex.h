#ifndef VERTEX_H
#define VERTEX_H
#include "luck.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Color4.h"
namespace luck { namespace core
{
    struct Vertex
    {
        f32 x, y, z;
        f32 r, g, b, a;
        f32 u, v;
        Vertex(f32 x, f32 y, f32 z, f32 r, f32 g, f32 b, f32 a, f32 u, f32 v) : x(x), y(y), z(z), r(r), g(g), b(b), a(a), u(u), v(v)
        { }
        Vertex(Vector3<f32> position, Color4 color, Vector2<f32> uv) :
            x(position.x), y(position.y), z(position.z), r(color.r), g(color.g), b(color.b), a(color.a), u(uv[0]), v(uv[1])
        { }
        Vertex() = default;
    };
}}
#endif

