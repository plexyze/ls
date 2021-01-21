//
// Created by d on 18.12.2020.
//

#pragma once

#include "ls/device/log.h"
#include <sstream>

#define IS_ERROR_LOG_ENABLED 1
#define IS_INFO_LOG_ENABLED 1


/**
 * Android Log.
 */
class AndroidLog:public ls::Log{

public:
    void e(const string &tag, const stringstream &msg) override;

    void i(const string &tag, const stringstream &msg) override;
};

