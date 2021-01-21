//
// Created by d on 16.12.2020.
//

#pragma once

#include "renderer.h"
#include "../3d/mesh.h"
#include "../device/log.h"
#include "../3d/mat4.h"
#include "../3d/angle.h"
#include "../3d/mat_transform.h"

#include <android/log.h>
#include <math.h>

// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif




namespace ls{
    class OpenGlEs3 : public Renderer {
    private:
        static string LOG_TAG;

    private:
        std::shared_ptr<Log> log;
        int width = 0;
        int height = 0;
        GLuint programObject = 0;
        Mat4f mPerspective = Mat4f(1);
        Mat4f mTransform = Mat4f(1);
        Mesh mash;
    private:
        bool checkGlError(const char* funcName);
        GLuint createShader(GLenum shaderType, const char* src);
        GLuint createProgram(const char* vtxSrc, const char* fragSrc);
        void printGlString(const char* name, GLenum s);
    public:
        OpenGlEs3(const std::shared_ptr<Log>& log){
            this->log = log;
        }

        void init() override ;

        void resize(int w, int h) override;

        bool draw() override;
    };
}


#include "opengles3.inl"
