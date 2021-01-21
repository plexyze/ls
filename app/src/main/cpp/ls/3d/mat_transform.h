//
// Created by d on 17.12.2020.
//

#ifndef LS_MAT_TRANSFORM_H
#define LS_MAT_TRANSFORM_H
#include "vec4.h"
#include "vec3.h"
#include "mat4.h"

namespace ls{
    template<typename T>
    Mat4<T> translate(const Mat4<T> &m, const Vec3<T>& v){
        return Mat4<T>(
                m.x,
                m.y,
                m.z,
                m.x * v.x + m.y * v.y + m.z * v.z + m.w
                );
    }

    template<typename T>
    Mat4<T> scale(const Mat4<T> &m, const Vec3<T>& v){
        return Mat4<T>(
                m.x * v.x,
                m.y * v.y,
                m.z * v.z,
                m.w
                );
    }

    template<typename T>
    Mat4<T> rotate(const Mat4<T> &m, const T angle, const Vec3<T>& v){
        T const a = angle;
        T const c = cos(a);
        T const s = sin(a);

        Vec3<T> axis = v.normalize();
        Vec3<T> temp(axis * (T(1) - c));

        Mat4<T> Rotate;
        Rotate.x.x = c + temp.x * axis.x;
        Rotate.x.y = temp.x * axis.y + s * axis.z;
        Rotate.x.z = temp.x * axis.z - s * axis.y;

        Rotate.y.x = temp.y * axis.x - s * axis.z;
        Rotate.y.y = c + temp.y * axis.y;
        Rotate.y.z = temp.y * axis.z + s * axis.x;

        Rotate.z.x = temp.z * axis.x + s * axis.y;
        Rotate.z.y = temp.z * axis.y - s * axis.x;
        Rotate.z.z = c + temp.z * axis.z;

        Mat4<T> Result;
        Result.x = m.x * Rotate.x.x + m.y * Rotate.x.y + m.z * Rotate.x.z;
        Result.y = m.x * Rotate.y.x + m.y * Rotate.y.y + m.z * Rotate.y.z;
        Result.z = m.x * Rotate.z.x + m.y * Rotate.z.y + m.z * Rotate.z.z;
        Result.w = m.w;
        return Result;
    }

    template<typename T>
    Mat4<T> perspective(const T& fovy, const T& aspect, const T& zNear, const T& zFar){
        T range = tan(radians(fovy / T(2))) * zNear;
        T left = -range * aspect;
        T right = range * aspect;
        T bottom = -range;
        T top = range;

        Mat4<T> result;
        result.x.x = (T(2) * zNear) / (right - left);
        result.y.y = (T(2) * zNear) / (top - bottom);
        result.z.z = - (zFar + zNear) / (zFar - zNear);
        result.z.w = - T(1);
        result.w.z = - (T(2) * zFar * zNear) / (zFar - zNear);
        return result;

    }


}
#endif //LS_MAT_TRANSFORM_H
