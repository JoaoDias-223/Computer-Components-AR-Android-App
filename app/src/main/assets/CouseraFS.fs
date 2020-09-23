precision lowp float;
uniform vec4 uColor; //default color

varying float vDiffuseLightWeighting;
varying float vSpecularLightWeighting;

varying vec3 vAmbientLight;
varying vec3 vDiffuseLight;
varying vec3 vSpecularLight;

varying float vAlpha;
uniform int uHasTexture;
varying vec2 vTextureCoordinate;  //texture
uniform sampler2D uTextureSampler;

void main(){
    vec4 fragmentColor = uColor;
    if (uHasTexture == 1)
        fragmentColor = texture2D(uTextureSampler, vec2(vTextureCoordinate.s, 1.0-vTextureCoordinate.t));

    vec4 diffuseColor = vDiffuseLightWeighting * vec4(vDiffuseLight, 1.0);
    vec4 specularColor = vSpecularLightWeighting * vec4(vSpecularLight, 1);

    if (uHasTexture == 1)
        gl_FragColor = vec4(fragmentColor.xyz, vAlpha);
    else
        gl_FragColor = vec4(fragmentColor.xyz, vAlpha) + vec4(vAmbientLight, 0) + specularColor+diffusseColor;
}