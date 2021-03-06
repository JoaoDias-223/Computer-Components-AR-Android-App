package com.android.gles3jni;

public class Material {
    private String Name;
    private float[] Ambient;
    private float[] Diffuse;
    private float[] Specular;
    private float SpecularCoefficient;

    public Material(String name, float[] ambient, float[] diffuse, float[] specular, float specularCoefficient){
        Name = name;
        Ambient = ambient;
        Diffuse = diffuse;
        Specular = specular;
        SpecularCoefficient = specularCoefficient;
    }

    public String getName() {
        return Name;
    }

    public float[] getAmbient() {
        return Ambient;
    }

    public float[] getDiffuse() {
        return Diffuse;
    }

    public float[] getSpecular() {
        return Specular;
    }

    public float getSpecularCoefficient() {
        return SpecularCoefficient;
    }
}
