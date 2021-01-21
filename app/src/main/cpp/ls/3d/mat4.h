//
// Created by d on 16.12.2020.
//

#ifndef LS_MAT4_H
#define LS_MAT4_H
#include "vec4.h"

namespace ls {

    template<typename T>
    struct Mat4 {
        union {
            struct {Vec4<T>a,b,c,d;};
            struct {Vec4<T>x,y,z,w;};
        };

        Mat4(const float v = 0)
            :a(v,0,0,0),
             b(0,v,0,0),
             c(0,0,v,0),
             d(0,0,0,v){}

        Mat4(const Vec4<T>& a,const Vec4<T>& b, const Vec4<T>& c, const Vec4<T>& d)
                :a(a), b(b), c(c), d(d){}


        Mat4(const Mat4& m){
            a = m.a;
            b = m.b;
            c = m.c;
            d = m.d;
        }

        Mat4& operator=(const Mat4& m){
            a = m.a;
            b = m.b;
            c = m.c;
            d = m.d;
            return *this;
        }

        Mat4 operator*(const Mat4& m){
            return Mat4<T> (
                x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w
            );
        }


    };

    typedef Mat4<float> Mat4f;
    typedef Mat4<double> Mat4d;

}
#endif //LS_MAT4_H
