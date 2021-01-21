//
// Created by d on 16.12.2020.
//

#ifndef LS_VEC3_H
#define LS_VEC3_H

namespace ls{
    template<typename T>
    struct Vec3{
        union {
            struct {T x, y, z; };
            struct {T r, g, b; };
            struct {T v0, v1, v2; };
        };

        Vec3(const Vec3& v){
            x = v.x;
            y = v.y;
            z = v.z;
        }

        Vec3& operator =(const Vec3& v){
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        Vec3(const T v = 0){
            x = y = z  = v;
        }

        Vec3(const T x, const T y, const T z):x(x),y(y),z(z){}

        Vec3 operator*(const Vec3& v){
            return Vec3(x*v.x, y*v.y, z*v.z);
        }

        Vec3 operator+(const Vec3& v){
            return Vec3(x+v.x, y+v.y, z+v.z);
        }

        Vec3 normalize()const{
            T locLength = sqrt((x * x) + (y * y) + (z * z));
            T inv_length = (1 / locLength);
            return Vec3(x * inv_length,y * inv_length,z * inv_length);
        }
    };

    typedef Vec3<float> Vec3f;
    typedef Vec3<double> Vec3d;
}

#endif //LS_VEC3_H
