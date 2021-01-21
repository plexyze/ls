//
// Created by d on 16.12.2020.
//

#ifndef LS_VEC4_H
#define LS_VEC4_H
namespace ls{
    template<typename T>
    struct Vec4{
        union {
            struct {T x, y, z, w; };
            struct {T r, g, b, a; };
        };

        Vec4(const Vec4& v){
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }

        Vec4& operator =(const Vec4& v){
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        Vec4(const T v = 0){
            x = y = z = w = v;
        }

        Vec4(const T x, const T y, const T z, const T w):x(x),y(y),z(z),w(w){}

        Vec4 operator*(const Vec4& v)const{
            return Vec4(x*v.x, y*v.y, z*v.z, w*v.w);
        }

        Vec4 operator+(const Vec4& v)const{
            return Vec4(x+v.x, y+v.y, z+v.z, w+v.w);
        }
    };

    typedef Vec4<float> Vec4f;
    typedef Vec4<double> Vec4d;
}
#endif //LS_VEC4_H
