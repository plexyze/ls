//
// Created by d on 14.12.2020.
//
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lsjni.h"
#include "log.h"

#include "ls/app/application.h"
#include "ls/ui/opengles3.h"
#include "ls/core/device.h"
#include "start_actor.h"


#include <thread>

using namespace std;
using namespace ls;



extern "C" {
JNIEXPORT void JNICALL Java_com_plexyze_ls_GLES3JNILib_init(JNIEnv* env, jclass obj);
JNIEXPORT void JNICALL Java_com_plexyze_ls_GLES3JNILib_resize(JNIEnv* env, jclass obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_plexyze_ls_GLES3JNILib_step(JNIEnv* env, jclass obj);
};


JNIEXPORT void JNICALL
Java_com_plexyze_ls_GLES3JNILib_init(JNIEnv* env, jclass obj) {

    if(!Application::renderer){

        auto log = shared_ptr<AndroidLog>(new AndroidLog());

        Device::init(log);
        Device::startActor(new TestSandboxActor(log.get()));

        Application::renderer = shared_ptr<Renderer>(new OpenGlEs3(log));
    }

    Application::renderer->init();
}

JNIEXPORT void JNICALL
Java_com_plexyze_ls_GLES3JNILib_resize(JNIEnv* env, jclass obj, jint width, jint height) {
    Application::renderer->resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_plexyze_ls_GLES3JNILib_step(JNIEnv* env, jclass obj) {
    Device::uiUpdate();
    Application::renderer->draw();
}


