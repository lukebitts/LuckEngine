#ifndef COLOR4_H
#define COLOR4_H
namespace luck { namespace core
{
    ///@todo Remove template from color, color only has 1 type (u8 casted to f32 and divided by 255 anytime you want to use it with opengl)
    template <typename T>
    struct Color4
    {
        T a;
        T r;
        T g;
        T b;
        Color4()
        {
            a = r = g = b = (T)0;
        }
        Color4(T r, T g, T b, T a)
        {
            this->a = a;
            this->r = r;
            this->g = g;
            this->b = b;
        }
    };
}}
#endif
