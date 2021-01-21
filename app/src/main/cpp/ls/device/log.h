//
// Created by d on 18.12.2020.
//

#pragma once

#include <cstdint>
#include <string>

using namespace std;

namespace ls{

    class Log{
    public:
        virtual void e(const string& tag, const stringstream& msg) {}
        virtual void i(const string& tag, const stringstream& msg) {}
    };
}


