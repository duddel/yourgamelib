#version 300 es
precision mediump float;

in vec3 vOutPos;
in vec3 vOutNorm;
in vec3 vOutCol;

out vec4 color;

void main()
{
    vec3 lightPosition = vec3(50.0, 80.0, 50.0);
    vec3 lightDir = normalize(lightPosition - vOutPos);
    float diff = max(dot(normalize(vOutNorm), lightDir), 0.0);
    color = vec4(vOutCol * (diff + 0.2), 1.0);
}
