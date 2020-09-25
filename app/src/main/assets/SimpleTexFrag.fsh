#version 300 es

precision mediump float;

out vec4 FragColor;
in vec2 UV;

uniform sampler2D textureSampler;

void main() {

    vec4 color;
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 tex = texture(textureSampler, UV);
    if (UV.x == 0.0 || UV.y == 0.0){
        color = red;
    }
    else{
        color = tex;
    }

    FragColor = color;
}
