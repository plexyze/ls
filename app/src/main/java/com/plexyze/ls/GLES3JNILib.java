package com.plexyze.ls;

public class GLES3JNILib {
    static {
        System.loadLibrary("lsjni");
    }
    public static native void init();
    public static native void resize(int width, int height);
    public static native void step();
}