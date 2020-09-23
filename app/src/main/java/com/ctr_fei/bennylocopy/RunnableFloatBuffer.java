package com.ctr_fei.bennylocopy;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

public class RunnableFloatBuffer implements Runnable{

    float[] floatArray;
    float scalefactor;
    private FloatBuffer floatBuffer;

    public RunnableFloatBuffer(float[] floatArray, float scalefactor){
        this.floatArray = floatArray;
        this.scalefactor = scalefactor;
    }

    @Override
    public void run() {
        scale3dModel(scalefactor, floatArray);
        this.floatBuffer = setFBuffer(floatArray);
    }

    //utils

    private FloatBuffer setFBuffer(float[] array){
        ByteBuffer vbb = ByteBuffer.allocateDirect(array.length * 4);
        vbb.order(ByteOrder.nativeOrder());
        FloatBuffer floatBuffer = vbb.asFloatBuffer();
        floatBuffer.put(array);
        vbb.position(0);
        floatBuffer.position(0);

        return floatBuffer;
    }

    private void scale3dModel(float ScaleFactor, float[] array){
        for (int i = 0; i < array.length; i++){
            array[i] = array[i] * ScaleFactor;
        }
    }

    //getters

    public float[] getFloatArray() {
        return floatArray;
    }

    public float getScalefactor() {
        return scalefactor;
    }

    public FloatBuffer getFloatBuffer() {
        return floatBuffer;
    }

    //setters

    public void setFloatArray(float[] floatArray) {
        this.floatArray = floatArray;
    }

    public void setScalefactor(float scalefactor) {
        this.scalefactor = scalefactor;
    }

    public void setFloatBuffer(FloatBuffer floatBuffer) {
        this.floatBuffer = floatBuffer;
    }
}
