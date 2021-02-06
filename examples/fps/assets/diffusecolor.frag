// meant to be compatible with glsl 330 and 300 es
// desired #version has to be prepended befor compiling

precision mediump float; // required for es

in vec3 vOutPos;
in vec3 vOutNorm;
in vec3 vOutCol;

layout(location = 0) out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightDiffuse;

void main()
{
    vec3 lightDir = normalize(lightPos - vOutPos);
    float diff = max(dot(normalize(vOutNorm), lightDir), 0.0);

    color = vec4(vOutCol * lightDiffuse * (diff + 0.2), 1.0);
}
