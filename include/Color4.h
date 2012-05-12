#ifndef COLOR4_H
#define COLOR4_H
namespace luck { namespace core
{
    struct Color4
    {
        f32 a;
        f32 r;
        f32 g;
        f32 b;
        Color4()
        {
            a = r = g = b = 1.f;
        }
        Color4(f32 r, f32 g, f32 b, f32 a)
        {
            this->a = a;
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };
}}
#endif
