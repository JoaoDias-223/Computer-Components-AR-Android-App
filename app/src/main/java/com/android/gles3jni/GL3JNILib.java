package com.android.gles3jni;

// Wrapper for native library

import android.content.res.AssetManager;

public class GL3JNILib {
    /**
     * @param width the current view width
     * @param height the current view height
     */
    public static native void init(int width, int height);
    public static native void step();

    //pass the image to JNI C++ side
    public static native void setImage(long imageRGBA);

    public static native void toggleFeatures();
    public static native void setNewImage(long imageRGBA);

    public static native void setWidthAndHeight(int width, int height);
    public static native boolean isReadyToDraw();

}