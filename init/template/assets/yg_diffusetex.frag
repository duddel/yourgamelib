// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutPos;
in vec3 vOutNorm;
in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform sampler2D textureDiffuse;

void main()
{
    vec3 lightDir = normalize(lightPosition - vOutPos);
    float diff = max(dot(normalize(vOutNorm), lightDir), 0.0);
    vec3 finalLight = lightAmbient + (lightDiffuse * diff);

    vec3 finalColor = vec3(texture(textureDiffuse, vOutTex));

    color = vec4(finalColor * finalLight, 1.0);
}
