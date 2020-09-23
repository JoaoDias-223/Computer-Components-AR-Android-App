package com.ctr_fei.bennylocopy;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

public class Background {
    private static final String TAG = "Background";

    private FloatBuffer vertexBuffer, textureBuffer;
    private ShortBuffer drawListBuffer;
    private final int mProgram;
    private int mPositionHandle;
    private int mColorHandle;
    private int mTextureCoordHandle;
    private Shader VertShader;
    private Shader FragShader;

    static final int COORDS_PER_VERTEX = 3;

    static float squareVertices[] = {
            -1.0f,   1.0f,  1.0f, //bottom left
            -1.0f,  -1.0f,  1.0f, //upper left
             1.0f,  -1.0f,  1.0f, //upper right
             1.0f,   1.0f,  1.0f, //bottom right
    };

    private short drawOrder[] = { 0, 1, 2, 0, 2, 3 };

    static float textureVertices[] = {
            0.0f,  1.0f,  1.0f,  //bottom left
            1.0f,  1.0f,  1.0f,  //upper left
            1.0f,  0.0f,  1.0f,  //upper right
            0.0f,  0.0f,  1.0f,  //bottom right
    };

    private final int VERTEX_STRIDE = COORDS_PER_VERTEX * 4;

    private int texture;

    public Background(int texture, Context context){
        this.texture = texture;

        ByteBuffer bb = ByteBuffer.allocateDirect(squareVertices.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(squareVertices);
        vertexBuffer.position(0);

        ByteBuffer dlb = ByteBuffer.allocateDirect(drawOrder.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);

        ByteBuffer bb2 = ByteBuffer.allocateDirect(textureVertices.length * 4);
        bb2.order(ByteOrder.nativeOrder());
        textureBuffer = bb2.asFloatBuffer();
        textureBuffer.put(textureVertices);
        textureBuffer.position(0);

        VertShader = new Shader(context, "SimpleBackgroundVert.vsh");
        FragShader = new Shader(context, "SimpleBackgroundFrag.fsh");

        int vertexShader = J_OpenGLRenderer.loadShader(GLES20.GL_VERTEX_SHADER, VertShader.getShaderCode());
        int fragmentShader = J_OpenGLRenderer.loadShader(GLES20.GL_FRAGMENT_SHADER, FragShader.getShaderCode());

        mProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(mProgram, vertexShader);
        GLES20.glAttachShader(mProgram, fragmentShader);
        GLES20.glLinkProgram(mProgram);
        GLES20.glValidateProgram(mProgram);
    }

    public void draw()
    {
        //GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture);

        init();

        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer);

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle);
        GLES20.glDisableVertexAttribArray(mTextureCoordHandle);
    }

    private void init(){
        GLES20.glUseProgram(mProgram);

        mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
        GLES20.glEnableVertexAttribArray(mPositionHandle);
        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, VERTEX_STRIDE, vertexBuffer);

        mTextureCoordHandle = GLES20.glGetAttribLocation(mProgram, "inputTextureCoordinate");
        GLES20.glEnableVertexAttribArray(mTextureCoordHandle);
        GLES20.glVertexAttribPointer(mTextureCoordHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, VERTEX_STRIDE, textureBuffer);

        mColorHandle = GLES20.glGetUniformLocation(mProgram, "s_texture");
    }
}
