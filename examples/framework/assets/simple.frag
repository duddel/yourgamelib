#version 330


in vec3 vOutPos;
in vec3 vOutNorm;
out vec3 color;
uniform float light;

void main()
{
    color = (vOutNorm + 1.0) * 0.5 * light;
}
