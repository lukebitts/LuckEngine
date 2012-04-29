#ifndef COLOR4_H
#define COLOR4_H
namespace luck { namespace core
{
    struct Color4
    {
        u8 a;
        u8 r;
        u8 g;
        u8 b;
        Color4()
        {
            a = r = g = b = 255;
        }
        Color4(u8 r, u8 g, u8 b, u8 a)
        {
            this->a = a;
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };
}}
#endif
