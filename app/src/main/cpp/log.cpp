//
// Created by d on 18.12.2020.
//

#include <android/log.h>
#include "log.h"

void AndroidLog::e(const string &tag, const stringstream &msg) {
#if IS_ERROR_LOG_ENABLED
    string out = msg.str();
    __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), "%s", out.c_str());
#endif
}

void AndroidLog::i(const string &tag, const stringstream &msg) {
#if IS_INFO_LOG_ENABLED
    string out = msg.str();
    __android_log_print(ANDROID_LOG_INFO, tag.c_str(), "%s", out.c_str());
#endif
}
