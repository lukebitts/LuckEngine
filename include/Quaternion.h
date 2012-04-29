#ifndef QUATERNION_H
#define QUATERNION_H
#define TOLERANCE 0.00001f
#define PIOVER180 3.14/180
#include <math.h>
#include "Vector3.h"
namespace luck { namespace core {
    template <typename T>
    struct Quaternion
    {
        T x;
        T y;
        T z;
        T w;
        void normalize()
        {
            // Don't normalize if we don't have to
            T mag2 = w * w + x * x + y * y + z * z;
            if (fabs(mag2) > TOLERANCE && fabs(mag2 - 1.0f) > TOLERANCE) {
                T mag = sqrt(mag2);
                w /= mag;
                x /= mag;
                y /= mag;
                z /= mag;
            }
        }
        Quaternion<T> getConjugate()
        {
            return Quaternion(-x, -y, -z, w);
        }
        Quaternion<T> operator* (const Quaternion<T> &rq) const
        {
            // the constructor takes its arguments as (x, y, z, w)
            return Quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
                              w * rq.y + y * rq.w + z * rq.x - x * rq.z,
                              w * rq.z + z * rq.w + x * rq.y - y * rq.x,
                              w * rq.w - x * rq.x - y * rq.y - z * rq.z);
        }
        Vector3<T> operator* (const Vector3<T> &vec) const
        {
            Vector3<T> vn(vec);
            vn.normalize();

            Quaternion<T> vecQuat, resQuat;
            vecQuat.x = vn.x;
            vecQuat.y = vn.y;
            vecQuat.z = vn.z;
            vecQuat.w = 0.0f;

            resQuat = vecQuat * getConjugate();
            resQuat = *this * resQuat;

            return (Vector3<T>(resQuat.x, resQuat.y, resQuat.z));
        }
        void FromAxis(const Vector3<T> &v, float angle)
        {
            T sinAngle;
            angle *= 0.5f;
            Vector3<T> vn(v);
            vn.normalize();

            sinAngle = sinf(angle);

            x = (vn.x * sinAngle);
            y = (vn.y * sinAngle);
            z = (vn.z * sinAngle);
            w = cos(angle);
        }
        void FromEuler(T pitch, T yaw, T roll)
        {
            // Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
            // and multiply those together.
            // the calculation below does the same, just shorter

            T p = pitch * PIOVER180 / 2.0;
            T y = yaw * PIOVER180 / 2.0;
            T r = roll * PIOVER180 / 2.0;

            T sinp = sin(p);
            T siny = sin(y);
            T sinr = sin(r);
            T cosp = cos(p);
            T cosy = cos(y);
            T cosr = cos(r);

            this->x = sinr * cosp * cosy - cosr * sinp * siny;
            this->y = cosr * sinp * cosy + sinr * cosp * siny;
            this->z = cosr * cosp * siny - sinr * sinp * cosy;
            this->w = cosr * cosp * cosy + sinr * sinp * siny;

            normalize();
        }
        /*Matrix4 Quaternion::getMatrix() const
        {
            float x2 = x * x;
            float y2 = y * y;
            float z2 = z * z;
            float xy = x * y;
            float xz = x * z;
            float yz = y * z;
            float wx = w * x;
            float wy = w * y;
            float wz = w * z;

            // This calculation would be a lot more complicated for non-unit length quaternions
            // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
            //   OpenGL
            return Matrix4( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
                    2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
                    2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f)
        }*/
        void getAxisAngle(Vector3<T> *axis, float *angle)
        {
            T scale = sqrt(x * x + y * y + z * z);
            axis->x = x / scale;
            axis->y = y / scale;
            axis->z = z / scale;
            *angle = acos(w) * 2.0f;
        }

    };
}}
#endif
