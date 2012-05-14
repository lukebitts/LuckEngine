#ifndef MATRIX4X4_H
#define MATRIX4X4_H
#include "luck.h"
#include "Vector3.h"
#include "lmath.h"
namespace luck { namespace core
{
    template<typename T>
    struct Matrix4x4
    {
        private:
            T mat4x4[4][4];
        public:
            T* operator[](u16 const n)
            {
                return mat4x4[n];
            }
            T at(u16 const a, u16 const b) const
            {
                return mat4x4[a][b];
            }
            Matrix4x4()
            {
                std::fill_n(&mat4x4[0][0], 4*4, (T)0);
            }
            Matrix4x4(const Matrix4x4<T>& c)
            {
                for(u16 i = 0; i < 4; i++)
                for(u16 j = 0; j < 4; j++)
                    mat4x4[i][j] = c.at(i,j);
            }
            explicit Matrix4x4(T n)
            {
                mat4x4[0][0] = n; mat4x4[0][1] = 0; mat4x4[0][2] = 0; mat4x4[0][3] = 0;
                mat4x4[1][0] = 0; mat4x4[1][1] = n; mat4x4[1][2] = 0; mat4x4[1][3] = 0;
                mat4x4[2][0] = 0; mat4x4[2][1] = 0; mat4x4[2][2] = n; mat4x4[2][3] = 0;
                mat4x4[3][0] = 0; mat4x4[3][1] = 0; mat4x4[3][2] = 0; mat4x4[3][3] = 1;
            }
            Matrix4x4(T a1, T a2, T a3, T a4,
                      T b1, T b2, T b3, T b4,
                      T c1, T c2, T c3, T c4,
                      T d1, T d2, T d3, T d4)
            {
                mat4x4[0][0] = a1; mat4x4[0][1] = a2; mat4x4[0][2] = a3; mat4x4[0][3] = a4;
                mat4x4[1][0] = b1; mat4x4[1][1] = b2; mat4x4[1][2] = b3; mat4x4[1][3] = b4;
                mat4x4[2][0] = c1; mat4x4[2][1] = c2; mat4x4[2][2] = c3; mat4x4[2][3] = c4;
                mat4x4[3][0] = d1; mat4x4[3][1] = d2; mat4x4[3][2] = d3; mat4x4[3][3] = d4;
            }
            Vector3<T> operator*(const Vector3<T>& c)
            {
                return Vector3<T>( mat4x4[0][0] * c.x + mat4x4[0][1] * c.y + mat4x4[0][2] * c.z + mat4x4[0][3] * c.w,
                                   mat4x4[1][0] * c.x + mat4x4[1][1] * c.y + mat4x4[1][2] * c.z + mat4x4[1][3] * c.w,
                                   mat4x4[2][0] * c.x + mat4x4[2][1] * c.y + mat4x4[2][2] * c.z + mat4x4[2][3] * c.w,
                                   mat4x4[3][0] * c.x + mat4x4[3][1] * c.y + mat4x4[3][2] * c.z + mat4x4[3][3] * c.w );
            }
            Matrix4x4<T> operator*(const Matrix4x4<T>& c) const
            {
                return Matrix4x4<T>(this->at(0,0)*c.at(0,0)+this->at(0,1)*c.at(1,0)+this->at(0,2)*c.at(2,0)+this->at(0,3)*c.at(3,0),
                                    this->at(0,0)*c.at(0,1)+this->at(0,1)*c.at(1,1)+this->at(0,2)*c.at(2,1)+this->at(0,3)*c.at(3,1),
                                    this->at(0,0)*c.at(0,2)+this->at(0,1)*c.at(1,2)+this->at(0,2)*c.at(2,2)+this->at(0,3)*c.at(3,2),
                                    this->at(0,0)*c.at(0,3)+this->at(0,1)*c.at(1,3)+this->at(0,2)*c.at(2,3)+this->at(0,3)*c.at(3,3),

                                    this->at(1,0)*c.at(0,0)+this->at(1,1)*c.at(1,0)+this->at(1,2)*c.at(2,0)+this->at(1,3)*c.at(3,0),
                                    this->at(1,0)*c.at(0,1)+this->at(1,1)*c.at(1,1)+this->at(1,2)*c.at(2,1)+this->at(1,3)*c.at(3,1),
                                    this->at(1,0)*c.at(0,2)+this->at(1,1)*c.at(1,2)+this->at(1,2)*c.at(2,2)+this->at(1,3)*c.at(3,2),
                                    this->at(1,0)*c.at(0,3)+this->at(1,1)*c.at(1,3)+this->at(1,2)*c.at(2,3)+this->at(1,3)*c.at(3,3),

                                    this->at(2,0)*c.at(0,0)+this->at(2,1)*c.at(1,0)+this->at(2,2)*c.at(2,0)+this->at(2,3)*c.at(3,0),
                                    this->at(2,0)*c.at(0,1)+this->at(2,1)*c.at(1,1)+this->at(2,2)*c.at(2,1)+this->at(2,3)*c.at(3,1),
                                    this->at(2,0)*c.at(0,2)+this->at(2,1)*c.at(1,2)+this->at(2,2)*c.at(2,2)+this->at(2,3)*c.at(3,2),
                                    this->at(2,0)*c.at(0,3)+this->at(2,1)*c.at(1,3)+this->at(2,2)*c.at(2,3)+this->at(2,3)*c.at(3,3),

                                    this->at(3,0)*c.at(0,0)+this->at(3,1)*c.at(1,0)+this->at(3,2)*c.at(2,0)+this->at(3,3)*c.at(3,0),
                                    this->at(3,0)*c.at(0,1)+this->at(3,1)*c.at(1,1)+this->at(3,2)*c.at(2,1)+this->at(3,3)*c.at(3,1),
                                    this->at(3,0)*c.at(0,2)+this->at(3,1)*c.at(1,2)+this->at(3,2)*c.at(2,2)+this->at(3,3)*c.at(3,2),
                                    this->at(3,0)*c.at(0,3)+this->at(3,1)*c.at(1,3)+this->at(3,2)*c.at(2,3)+this->at(3,3)*c.at(3,3));
            }
            static Matrix4x4<T> translationMatrix(T x, T y, T z)
            {
                return Matrix4x4<T>(1, 0, 0, x,
                                    0, 1, 0, y,
                                    0, 0, 1, z,
                                    0, 0, 0, 1);
            }
            static Matrix4x4<T> scaleMatrix(T x, T y, T z)
            {
                return Matrix4x4<T>(x, 0, 0, 0,
                                    0, y, 0, 0,
                                    0, 0, z, 0,
                                    0, 0, 0, 1);
            }
            static Matrix4x4<T> rotationMatrix(f32 angle, Vector3<T> toRotate)
            {
                toRotate.normalize();

                angle = angle * math::PI/180;

                T c = cosf(angle * toRotate.x);
                T s = sinf(angle * toRotate.x);
                Matrix4x4<T> rotX( 1, 0, 0, 0,
                                   0, c, s, 0,
                                   0,-s, c, 0,
                                   0, 0, 0, 1);

                c = cosf(angle * toRotate.y);
                s = sinf(angle * toRotate.y);
                Matrix4x4<T> rotY( c, 0,-s, 0,
                                   0, 1, 0 ,0,
                                   s, 0, c, 0,
                                   0, 0, 0, 1);

                c = cosf(angle * toRotate.z);
                s = sinf(angle * toRotate.z);
                Matrix4x4<T> rotZ( c, s, 0, 0,
                                  -s, c, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1);

                return rotZ * rotY * rotX;
            }
            static Matrix4x4<T> perspective(f32 fov, f32 aspect, f32 near, f32 far)
            {
                f32 size = near * tanf((fov * math::PI/180)/2);
                f32 left = -size;
                f32 right = size;
                f32 bottom = -size / aspect;
                f32 top = size / aspect;

                return Matrix4x4<T>(2*near/(right-left), 0, 0, 0,
                                    0, 2*near/(top-bottom), 0, 0,
                                    (right+left)/(right-left), (top+bottom)/(top-bottom), -(far+near)/(far-near), -1,
                                    0, 0,-(2*far*near)/(far-near), 0);
            }
    };
}}
#endif

/*
    float size = near * tanf(degreesToRadians(angle) / 2.0);
    float left = -size, right = size, bottom = -size / aspect, top = size / aspect;

    // First Column
    matrix[0] = 2 * near / (right - left);
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;

    // Second Column
    matrix[4] = 0.0;
    matrix[5] = 2 * near / (top - bottom);
    matrix[6] = 0.0;
    matrix[7] = 0.0;

    // Third Column
    matrix[8] = (right + left) / (right - left);
    matrix[9] = (top + bottom) / (top - bottom);
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1;

    // Fourth Column
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = -(2 * far * near) / (far - near);
    matrix[15] = 0.0;

    f32 xymax = near * tanf((fov * math::PI/180)/2);
    f32 ymin = -xymax;
    f32 xmin = -xymax;

    f32 width = xymax - xmin;
    f32 height = xymax - ymin;

    f32 depth = far - near;
    f32 q = -(far + near) / depth;
    f32 qn = -2 * (far * near) / depth;

    f32 w = 2 * near / width;
    w = w / aspect;
    f32 h = 2 * near / height;
*/
