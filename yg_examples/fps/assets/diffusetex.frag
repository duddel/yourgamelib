// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutPos;
in vec3 vOutNorm;
in vec2 vOutTex;

layout(location = 0) out vec4 color;

uniform vec3 lightDiffuse;
uniform vec3 lightPosition;
uniform sampler2D textureDiffuse;

void main()
{
    vec3 lightDir = normalize(lightPosition - vOutPos);
    float diff = max(dot(normalize(vOutNorm), lightDir), 0.0);
    
    color = vec4(vec3(texture(textureDiffuse, vOutTex)) * lightDiffuse * (diff + 0.2), 1.0);
}
