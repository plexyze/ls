package com.plexyze.ls;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.util.Timer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class GLES3JNIView extends GLSurfaceView {
    private static final String TAG = "GLES3JNI";
    private static final boolean DEBUG = true;

    public GLES3JNIView(Context context) {
        super(context);
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        //setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        setEGLConfigChooser(8, 8, 8, 8, 24, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        static long timer = System.currentTimeMillis();
        static long steps = 0;

        public void onDrawFrame(GL10 gl) {
            steps++;
            long nextTime = System.currentTimeMillis();
            double difTime = nextTime-timer;
            if(difTime>1000){
                Log.i("FPS",(1000.0/(difTime/steps))+"");
                timer = nextTime;
                steps = 0;
            }
            GLES3JNILib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLES3JNILib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            GLES3JNILib.init();
        }
    }
}