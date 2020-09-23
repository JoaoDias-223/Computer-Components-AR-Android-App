precision lowp float;
uniform vec4 uColor; //default color

varying float vDiffuseLightWeighting;
varying float vSpecularLightWeighting;

varying vec3 vAmbientLight;
varying vec3 vDiffuseLight;
varying vec3 vSpecularLight;
varying float vAlpha;

void main(){
    vec4 fragmentColor = uColor;

    vec4 diffuseColor = vDiffuseLightWeighting * vec4(vDiffuseLight, 1.0);
    vec4 specularColor = vSpecularLightWeighting * vec4(vSpecularLight, 1.0);

    gl_FragColor = vec4(fragmentColor.xyz, vAlpha) + vec4(vAmbientLight, 0) + specularColor + diffusseColor;
}