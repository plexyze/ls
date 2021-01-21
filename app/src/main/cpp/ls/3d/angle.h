//
// Created by d on 17.12.2020.
//

#ifndef LS_ANGLE_H
#define LS_ANGLE_H

namespace ls{
    static const double PI = 3.14159265358979323846;

    template <typename T>
    T radians(const T angle){
        return angle * PI/180;
    }
}

#endif //LS_ANGLE_H
