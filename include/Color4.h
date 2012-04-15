#ifndef COLOR4_H
#define COLOR4_H
namespace luck { namespace core
{
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
