#ifndef VECTOR3_H
#define VECTOR3_H
#include <math.h>
#include <iostream>
namespace luck { namespace core
{
    template <typename T>
    struct Vector3
    {
        private:
            T vec3[4];
        public:
            T& x;
            T& y;
            T& z;
            T& w;
            Vector3() : x(vec3[0]), y(vec3[1]), z(vec3[2]), w(vec3[3])
            {
                x = 0;
                y = 0;
                z = 0;
                w = 0;
            }
            Vector3(T x_, T y_, T z_, T w_ = 0) : x(vec3[0]), y(vec3[1]), z(vec3[2]), w(vec3[3])
            {
                x = x_;
                y = y_;
                z = z_;
                w = w_;
            }
            Vector3<T>& operator= (Vector3<T> const& c)
            {
                x = c.x;
                y = c.y;
                z = c.z;
                w = c.w;
                return *this;
            }
            T& operator[](const u16 n)
            {
                return vec3[n];
            }
        //private:

        /*static Vector3<T> sub(Vector3<T> v1, Vector3<T> v2)
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
        static T dot(Vector3<T> v1, Vector3<T> v2)
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }
        static Vector3<T> normalize(Vector3<T> v1)
        {
            Vector3<T> r(v1);
            r.normalize();
            return r;
        }
        void toString()
        {
            std::cout<<x<<" "<<y<<" "<<z<<"\n";
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
            if(x == 0 && y == 0 && z == 0) return;
            T len = sqrtf((x*x)+(y*y)+(z*z));
            x = x/len;
            y = y/len;
            z = z/len;
        }*/
    };
}}
#endif
