#ifndef VERTEX_H
#define VERTEX_H
namespace luck { namespace core
{
    struct Vertex
    {
        Vector3<f32> position;
        Color4 color;
        Vertex(f32 x, f32 y, f32 z, u8 r, u8 g, u8 b, u8 a)
        {
            position.x = x;
            position.y = y;
            position.z = z;
            color.a = a;
            color.r = r;
            color.g = g;
            color.b = b;
        }
        Vertex(Vector3<f32> position, Color4 color = Color4())
        {
            this->position = position;
            this->color = color;
        }
        Vertex()
        {
            position = Vector3<f32>();
            color = Color4();
        }
    };
}}
#endif

