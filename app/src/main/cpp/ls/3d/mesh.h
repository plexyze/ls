//
// Created by d on 16.12.2020.
//

#ifndef LS_MESH_H
#define LS_MESH_H

#include <vector>
#include "vec3.h"

namespace ls{
    class Mesh {
    private:
        std::vector<Vec3f> buffer;

    public:
        Mesh();

        void push_back(const Vec3f& v);
        Vec3f* data();
        void clear();


    };
}




#endif //LS_MESH_H
