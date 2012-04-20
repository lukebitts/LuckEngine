#ifndef VECTOR3_H
#define VECTOR3_H
#include <math.h>
namespace luck { namespace core
{
    template <typename T>
    struct Vector3
    {
        static Vector3<T> sub(Vector3<T> v1, Vector3<T> v2)
        {
            return Vector3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
        }
        static Vector3<T> sum(Vector3<T> v1, Vector3<T> v2)
        {
            return Vector3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
        }
        static Vector3<T> cross(Vector3<T> v1, Vector3<T> v2)
        {
            /// http://calculator.tutorvista.com/math/8/cross-product-calculator.html
            return Vector3<T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
        }
        static Vector3<T> normalize(Vector3<T> v1)
        {
            Vector3<T> ret;
            ret.x = v1.x; ret.y = v1.y; ret.z = v1.z;
            ret.normalize();
            return ret;
        }
        Vector3<T> operator-(const Vector3<T> &v)
        {
            return Vector3::sub(*this,v);
        }
        Vector3<T> operator-(const T& n)
        {
            return *this - Vector3<T>(n,n,n);
        }
        Vector3<T> operator+(const Vector3<T> &v)
        {
            return Vector3::sum(*this,v);
        }
        Vector3<T> operator+(const T& n)
        {
            return *this + Vector3<T>(n,n,n);
        }
        Vector3<T> operator-=(const Vector3<T> &v)
        {
            *this = *this - v;
        }
        Vector3<T> operator-=(const T& v)
        {
            *this = *this - Vector3<T>(v,v,v);
        }
        Vector3<T> operator+=(const Vector3<T> &v)
        {
            *this = *this + v;
        }
        Vector3<T> operator+=(const T& v)
        {
            *this = *this + Vector3<T>(v,v,v);
        }
        Vector3<T> cross(const Vector3<T> &v)
        {
            return Vector3::cross(*this, v);
        }
        void normalize()
        {
            /// http://www.fundza.com/vectors/normalize/index.html
            T len = sqrtf((x*x)+(y*y)+(z*z));
            x = x/len;
            y = y/len;
            z = z/len;
        }
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
