//
// Created by d on 16.12.2020.
//

#include "mesh.h"

void ls::Mesh::push_back(const ls::Vec3f &v) {
    buffer.push_back(v);
}

ls::Vec3f *ls::Mesh::data() {
    return buffer.data();
}

void ls::Mesh::clear() {
    buffer.clear();
}

ls::Mesh::Mesh() {}
