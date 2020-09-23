#version 100

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

attribute vec4 vPosition;
attribute vec3 vNormal;

//output
out vec3 Normal;
out vec3 FragPos;

void main() {
    gl_Position = uMVPMatrix * vPosition;
    Normal = vNormal;
    FragPos = vec3(uModelMatrix * vec4(vPosition));
}