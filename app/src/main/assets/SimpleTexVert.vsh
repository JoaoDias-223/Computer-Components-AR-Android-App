#version 300 es

uniform mat4 uMVPMatrix;

in vec4 vPosition;
in vec2 vTextureCoordinates;

out vec2 UV;

void main(){
    UV = vTextureCoordinates;
    gl_Position = uMVPMatrix * vPosition;
}