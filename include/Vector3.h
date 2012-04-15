#ifndef VECTOR3_H
#define VECTOR3_H
namespace luck { namespace core
{
    template <typename T>
    struct Vector3
    {
        Vector3()
        {
            x = (T)0;
            y = (T)0;
            z = (T)0;
        }
        Vector3(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        T x;
        T y;
        T z;
    };
}}
#endif
