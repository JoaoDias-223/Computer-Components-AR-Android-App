#version 300 es

precision mediump float;

out vec4 FragColor;
in vec2 UV;

uniform sampler2D textureSampler;
uniform sampler2D bumpMap;

void main() {

    vec4 color;
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);

    vec4 tex1 = texture(textureSampler, UV);
    vec4 tex2 = texture(bumpMap, UV);

    vec4 tex = mix(tex1, tex2, 1.0);

    if (UV.x == 0.0 || UV.y == 0.0){
        color = red;
    }
    else{
        color = tex;
    }

    FragColor = color;
}
